

#include "alock.hpp"


namespace rrr {

WaitDieALock::~WaitDieALock() {
    verify(!done_);
    done_ = true;
    std::list<lock_req_t>::iterator it = requests_.begin();
    verify(status_ != RLOCKED);
    for (; it != requests_.end(); it++) {
        if (it->status == lock_req_t::WAIT)
            it->no_callback();
    }
    requests_.clear();
}

uint64_t WaitDieALock::vlock(const std::function<void(uint64_t)> &yes_callback,
        const std::function<void(void)>& no_callback,
        type_t type,
        int64_t priority,
        const std::function<int(void)> &) {
    uint64_t id = get_next_id();
    if (done_) {
        no_callback();
        return id;
    }

    if (status_ == FREE
        || (status_ == RLOCKED && type == RLOCK && n_w_in_queue_ == 0)) {
        // acquire lock
        requests_.emplace_back(id, priority, type, yes_callback, no_callback);
        // XXX can we omit yes_callback || no_callback here ???

        if (type == RLOCK) {
            n_r_in_queue_++;
            read_acquire(requests_.back());
        }
        else {
            n_w_in_queue_++;
            write_acquire(requests_.back());
        }
    }
    else {
        wd_status_t wd = wait_die(type, priority);
        if (wd == WD_WAIT) { // wait
            requests_.emplace_back(id, priority, type, yes_callback,
                    no_callback);
            if (type == RLOCK) {
                n_r_in_queue_++;
            }
            else {
                n_w_in_queue_++;
            }
        }
        else { // die
            no_callback();
        }
    }

    //sanity_check();
    return id;
}

WaitDieALock::wd_status_t WaitDieALock::wait_die(type_t type, int64_t priority) {
    switch (type) {
        case WLOCK: // in order to wait, the coming request needs to have
                    // larger priority (less priority value) than all reqs in
                    // the queue
        {
            std::list<lock_req_t>::reverse_iterator rit = requests_.rbegin();
            for (; rit != requests_.rend(); rit++) {
                if (priority >= rit->priority) // can't use > here or deadlock
                    return WD_DIE;
                if (rit->type == WLOCK)  // since requests_ are ordered by
                                        // priority for write locks
                    break;
            }
            return WD_WAIT;
        }
        case RLOCK:
        {
            std::list<lock_req_t>::reverse_iterator rit = requests_.rbegin();
            for (; rit != requests_.rend(); rit++) {
                if (rit->type == WLOCK) { // check tha write req with largest priority
                    if (priority < rit->priority)
                        return WD_WAIT;
                    else
                        return WD_DIE;
                }
            }
            verify(0); // should be able to acquire the lock, no need to wait or die
        }
        default:
            verify(0);
    }
}

void WaitDieALock::abort(uint64_t id) {
    if (done_) {
        return;
    }

    int64_t n_w_before_this = 0;
    std::list<lock_req_t>::iterator it = requests_.begin();
    for (; it != requests_.end(); it++)
        if (it->id == id)
            break;
        else if (it->type == WLOCK)
            n_w_before_this++;

    if (it == requests_.end())
        return; // no request found matching the given id

    if (it->status == lock_req_t::WAIT) { // abort waiting request
        lock_req_t aborted_lock_req(*it);
        std::list<lock_req_t>::iterator next_it = requests_.erase(it);
        if (aborted_lock_req.type == RLOCK) {
            n_r_in_queue_--;
        }
        else {
            n_w_in_queue_--;
            if (n_w_before_this == 0) { // alock must be read locked
                                        // needs to approve all following read
                                        // requests till next write request
                verify(status_ == RLOCKED);
                std::vector<lock_req_t *> lock_reqs;
                for (; next_it != requests_.end(); next_it++) {
                    if (next_it->type == RLOCK) {
                        lock_reqs.push_back(&(*next_it));
                    }
                    else
                        break;
                }
                read_acquire(lock_reqs);
            }
        }
        aborted_lock_req.no_callback();
    }
    else { // unlock
        if (it->type == RLOCK) { // unlock a read lock
            n_r_in_queue_--;
            n_rlock_--;
            std::list<lock_req_t>::iterator next_it = requests_.erase(it);
            if (n_rlock_ == 0) {
                if (next_it == requests_.end()) { // empty queue
                    status_ = FREE;
                    verify(requests_.size() == 0);
                }
                else {
                    write_acquire(*next_it);
                }
            }
        }
        else { // unlock a write lock
            n_w_in_queue_--;
            std::list<lock_req_t>::iterator next_it = requests_.erase(it);
            if (next_it == requests_.end()) { // empty queue
                status_ = FREE;
                verify(requests_.size() == 0);
            }
            else if (next_it->type == WLOCK) { // acquire next write lock
                write_acquire(*next_it);
            }
            else { // acquire read locks
                std::vector<lock_req_t *> lock_reqs;
                for (; next_it != requests_.end(); next_it++) {
                    if (next_it->type == WLOCK)
                        break;
                    lock_reqs.push_back(&(*next_it));
                }
                read_acquire(lock_reqs);
                verify(status_ == RLOCKED);
            }
        }
    }
    //sanity_check();
}

WoundDieALock::~WoundDieALock() {
    verify(!done_);
    done_ = true;
    std::list<lock_req_t>::iterator it = requests_.begin();
    verify(status_ != RLOCKED);
    for (; it != requests_.end(); it++) {
        if (it->status == lock_req_t::WAIT)
            it->no_callback();
    }
    requests_.clear();
}

void WoundDieALock::wound_die(type_t type, int64_t priority) {
    switch (type) {
        case WLOCK:
        {
            std::list<lock_req_t>::reverse_iterator rit = requests_.rbegin();
            while (rit != requests_.rend()) {
                if (rit->priority >= priority) { // try wound it
                    int ret = wound(*rit);
                    if (0 == ret) { // wounded successfully
                        rit = erase(requests_, rit);
                        continue;
                    }
                }
                else {
                    if (rit->type == WLOCK) {
                        break;
                    }
                }
                rit++;
            }
            break;
        }
        case RLOCK:
        {
            std::list<lock_req_t>::reverse_iterator rit = requests_.rbegin();
            while (rit != requests_.rend()) {
                if (rit->priority < priority) {
                    break;
                }
                if (rit->type == WLOCK) { // try wound write lock
                    int ret = wound(*rit);
                    if (0 == ret) { // wounded successfully
                        rit = erase(requests_, rit);
                        continue;
                    }
                }
                rit++;
            }
            break;
        }
    }
}

uint64_t WoundDieALock::vlock(const std::function<void(uint64_t)> &yes_callback,
        const std::function<void(void)>& no_callback,
        type_t type,
        int64_t priority,
        const std::function<int(void)> &wound_callback) {

    uint64_t id = get_next_id();

    if (done_) {
        no_callback();
        return id;
    }

    wound_die(type, priority);

    requests_.emplace_back(id,
            priority,
            type,
            yes_callback,
            no_callback,
            wound_callback);

    lock_req_t &front = requests_.front();
    switch (status_) {
        case FREE:
        {
            if (front.type == WLOCK) {
                write_acquire(front);
            }
            else {
                std::vector<lock_req_t *> lock_reqs;
                std::list<lock_req_t>::iterator it = requests_.begin();
                for (; it != requests_.end(); it++)
                    if (it->type == RLOCK)
                        lock_reqs.push_back(&(*it));
                    else
                        break;
                read_acquire(lock_reqs);
            }
            break;
        }
        case RLOCKED:
        {
            verify(front.type == RLOCK && front.status == lock_req_t::LOCK);
            bool new_acquired = false;
            std::vector<lock_req_t *> lock_reqs;
            std::list<lock_req_t>::iterator it = requests_.begin();
            for (; it != requests_.end(); it++) {
                if (it->status == lock_req_t::LOCK) {
                    verify(it->type == RLOCK && new_acquired == false);
                }
                else if (it->type == RLOCK) {
                    lock_reqs.push_back(&(*it));
                    new_acquired = true;
                }
                else
                    break;
            }
            read_acquire(lock_reqs);
            break;
        }
        case WLOCKED:
            verify(front.type == WLOCK && front.status == lock_req_t::LOCK);
            break;
        default:
            verify(0);
    }

    //sanity_check();
    return id;
}

void WoundDieALock::abort(uint64_t id) {
    if (done_)
        return;

    int64_t n_w_before_this = 0;
    std::list<lock_req_t>::iterator it = requests_.begin();
    for (; it != requests_.end(); it++)
        if (it->id == id)
            break;
        else if (it->type == WLOCK)
            n_w_before_this++;

    if (it == requests_.end())
        return; // no request found matching the given id

    if (it->status == lock_req_t::WAIT) { // abort waiting request
        lock_req_t aborted_lock_req(*it);
        std::list<lock_req_t>::iterator next_it = requests_.erase(it);
        if (aborted_lock_req.type == WLOCK) {
            if (n_w_before_this == 0) { // alock must be read locked
                                        // needs to approve all following read
                                        // requests till next write request
                verify(status_ == RLOCKED);
                std::vector<lock_req_t *> lock_reqs;
                for (; next_it != requests_.end(); next_it++) {
                    if (next_it->type == RLOCK) {
                        lock_reqs.push_back(&(*next_it));
                    }
                    else
                        break;
                }
                read_acquire(lock_reqs);
            }
        }
        aborted_lock_req.no_callback();
    }
    else { // unlock
        if (it->type == RLOCK) { // unlock a read lock
            n_rlock_--;
            std::list<lock_req_t>::iterator next_it = requests_.erase(it);
            if (n_rlock_ == 0) {
                if (next_it == requests_.end()) { // empty queue
                    status_ = FREE;
                    verify(requests_.size() == 0);
                }
                else {
                    write_acquire(*next_it);
                }
            }
        }
        else { // unlock a write lock
            std::list<lock_req_t>::iterator next_it = requests_.erase(it);
            if (next_it == requests_.end()) { // empty queue
                status_ = FREE;
                verify(requests_.size() == 0);
            }
            else if (next_it->type == WLOCK) { // acquire next write lock
                write_acquire(*next_it);
            }
            else { // acquire read locks
                std::vector<lock_req_t *> lock_reqs;
                for (; next_it != requests_.end(); next_it++) {
                    if (next_it->type == WLOCK)
                        break;
                    lock_reqs.push_back(&(*next_it));
                }
                read_acquire(lock_reqs);
                verify(status_ == RLOCKED);
            }
        }
    }
    //sanity_check();
}

uint64_t TimeoutALock::vlock(const std::function<void(uint64_t)>& yes_callback,
        const std::function<void(void)>& no_callback,
        type_t type,
        int64_t,
        const std::function<int(void)> &) {


    //        safe_check();
    //        lock_.lock();

    auto id = get_next_id();

    //if (RandomGenerator::rand(1, 10000) <= 9900) {
    //	yes_callback(id);
    //} else {
    //	no_callback();
    //}
    //return id;

    // first push the request into the lock queue.
    requests_.emplace_back(id,
            type,
            yes_callback,
            no_callback,
            0);
    //    requests_.emplace_back(id, type);
    ALockReq &req = requests_.back();


    bool lockable = (status_ == FREE) ||
        (status_ == RLOCKED && type == RLOCK);



    if (lockable) {
        // then the lock can be
        // locked successfully.

        //	yes_callback(id);
        //	return id;

        if (type == RLOCK) {
            //	    yes_callback(id);
            //	    return id;

            status_ = RLOCKED;
            n_rlock_ ++;
        } else {
            //	    yes_callback(id);
            //	    return id;

            status_ = WLOCKED;
        }


        req.set_status(ALockReq::LOCK);



    } else if (tm_wait_ > 0) {
        // status is RLOCKED, type is WLOCK
        // or status is WLOCKED.
        req.set_status(ALockReq::WAIT);
        uint64_t tm_now = rrr::Time::now();
        uint64_t tm_out = tm_now + tm_wait_;
        auto& alarm = get_alarm_s();
        req.alarm_id_ = alarm.add(tm_out, [this, &req] () {
                this->do_timeout(req);
                });
    } else {
        // tm_wait_ = 0
        // do nothing, no callback after release the lock_;
        requests_.pop_back();
    }

    //        lock_.unlock();

    // because req might be freed already, cannot use req object.
    if (lockable) {
        //Log_info("lock req yes: %p", &req);
        yes_callback(id);
    } else if (tm_wait_ == 0) {

        no_callback();
    } else {
        // do nothing.
    }
    return id;
}

void TimeoutALock::abort(uint64_t id) {

    //status_ = FREE;
    //return;

    //safe_check();
    //	std::lock_guard<std::mutex> guard(lock_);
    //        lock_.lock();

    // find the lock request in the queue.
    auto it = requests_.begin();
    for (; it != requests_.end(); it++) {
        if ((*it).id_ == id) {
            break;
        }
    }

    // it's ok if not found.
    // maybe the caller called multiple times.
    if (it == requests_.end()) {
        // maybe this has been aborted before.
        //            lock_.unlock();
        return;
    }

    // found the request, different actions based on
    // the current state of the lock.
    std::vector<ALockReq*> lock_reqs;

    ALockReq& req = *it;
    if (req.cas_status(ALockReq::LOCK, ALockReq::UNLOCK)) {
        // this is currently locked.
        // it cannot be in the timeout queue.
        auto type = req.type_;
        if (type == RLOCK) {
            n_rlock_--;
        }
        if (n_rlock_ == 0) {
            status_ = FREE;
            //                tm_last_ = 0;
        }
        requests_.erase(it);

        if (status_ == FREE) {
            lock_all(lock_reqs);
        }
    } else if (req.cas_status(ALockReq::WAIT, ALockReq::ABORT)) {
        // cancel timeout.
        // the timeout function might be called.
        get_alarm_s().remove(req.alarm_id_);
        requests_.erase(it);
    } else if (req.get_status() == ALockReq::TIMEOUT) {
        requests_.erase(it);
    } else if (req.get_status() == ALockReq::UNLOCK) {
        verify(0);
    } else if (req.get_status() == ALockReq::ABORT) {
        verify(0);
    } else {
        verify(0);
    }
    //        lock_.unlock();

    for (auto& r: lock_reqs) {
        r->yes_callback_(r->id_);
    }

    return;
}


TimeoutALock::~TimeoutALock() {
    //    return;

    // free all the lockes and trigger timeout for those waiting.
    std::vector<std::function<void(void)>> tocall;
    //        lock_.lock();
    auto& alarm = get_alarm_s();
    auto it = requests_.begin();
    while ((it = requests_.begin()) != requests_.end()) {
        auto& req = *it;
        if (req.cas_status(ALockReq::LOCK, ALockReq::UNLOCK)) {
            // FIXME: Be careful for this !!!
            //verify(0);
        } else if (req.cas_status(ALockReq::WAIT, ALockReq::TIMEOUT)) {
            alarm.remove(req.alarm_id_);
            req.no_callback_();
        } else if (req.get_status() == ALockReq::TIMEOUT) {
        } else {
            verify(0);
        }
        requests_.erase(it);
    }

    //        lock_.unlock();
}


void ALockGroup::lock_all(const std::function<void(void)>& yes_cb,
        const std::function<void(void)>& no_cb) {




    verify(cas_status(INIT, WAIT) || cas_status(LOCK, WAIT));

    yes_callback_ = yes_cb;
    no_callback_ = no_cb;


    db_ = new DragonBall(tolock_.size(), [this] () {
            if (this->cas_status(WAIT, LOCK)) {
            this->yes_callback_();
            } else {
            verify(this->get_status() == TIMEOUT);
            this->no_callback_();
            this->abort_all_locked();
            }
            });

    decltype(tolock_) tmp;

    swap(tmp, tolock_);

    for(auto &p: tmp) {
        auto &alock = p.first;
        auto &type = p.second;

        auto y_cb = [this, alock] (uint64_t id) {
            //		this->mtx_locks_.lock();
            this->locked_[alock] = id;
            //		this->mtx_locks_.unlock();
            this->db_->trigger();
        };

        auto n_cb = [this] () {
            this->set_status(TIMEOUT);
            this->db_->trigger();
        };

        auto _wound_callback = [this, alock] () -> int {
            int ret = wound_callback_();
            if (ret == 0)
                locked_.erase(alock);
            return ret;
        };

        /*auto areq_id = */
        alock->lock(y_cb, n_cb, type, priority_, _wound_callback);
        //            alocks_[alock] = areq_id;
    }
    //        mtx_locks_.unlock();
}


} // namespace rrr
 