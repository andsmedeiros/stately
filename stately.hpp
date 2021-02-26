#ifndef STATELY_HPP
#define STATELY_HPP

namespace stately {
namespace {

template<class T_state, class T_stimulus = void, class ... T_rest>
class handler : public handler<T_state, T_rest ...> {  
protected:
    using handler<T_state, T_rest ...>::handle;

    virtual T_state &handle(T_stimulus &stm) noexcept { 
        return *dynamic_cast<T_state *>(this);
    }
};

template<class T_state>
class handler<T_state, void> {  
protected:
    virtual T_state &handle(void) noexcept { 
        return *dynamic_cast<T_state *>(this); 
    }
};

template<class ... T_stimuli>
class state_machine;

template<class ... T_stimuli>
class state : public handler<state<T_stimuli ...>, T_stimuli ...> {
    friend state_machine<T_stimuli ...>;
    using handler<state<T_stimuli ...>, T_stimuli ...>::handle;

    virtual bool will_transition(state<T_stimuli ...> &target) noexcept { return true; }
    virtual void did_transition(state<T_stimuli ...> &target) noexcept {  }
};

} /* anonymous namespace */

template<class ... T_stimuli>
struct state_machine {
    using state_type = state<T_stimuli ...>;

private: state_type *current_state;
public:
    state_machine(state_type &initial_state) noexcept : current_state(&initial_state) { 
        initial_state.did_transition(initial_state);
    }

    template<class T>
    void stimulate(T &&stm = T()) noexcept {
        state_type &new_state = this->current_state->handle(stm);
        if(&new_state == this->current_state) return;
        
        if(this->current_state->will_transition(new_state)) {
            auto &old_state = *this->current_state;
            this->current_state = &new_state;
            new_state.did_transition(old_state);
        }
    }
};

} /* namespace stately */

#endif /* STATELY_HPP */