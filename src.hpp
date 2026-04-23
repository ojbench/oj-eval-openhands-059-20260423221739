
#include<iostream>
#include<cstdio>
#include<cstring>
#include<cmath>

class BaseJudger {
public:
    BaseJudger (size_t time_limit , size_t memory_limit ,
                const char *answer) : time_limit_(time_limit), memory_limit_(memory_limit) {
        answer_ = new char[strlen(answer) + 1];
        strcpy(answer_, answer);
        score_ = 0;
    }

    virtual void Submit (size_t time , size_t memory , const char *output) = 0;

    size_t GetScore () const { return score_; }

    virtual ~BaseJudger () {
        delete[] answer_;
    };

protected:
    char *answer_;
    const size_t time_limit_;
    const size_t memory_limit_;
    size_t score_;

    virtual bool CheckAnswer (const char *output) const {
        // the output must equal to the answer
        return strcmp(answer_ , output) == 0;
    }
};

class ICPCJudger : public BaseJudger {
public:
    ICPCJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) {
            if (100 > score_) score_ = 100;
        }
    }
};

class OIJudger : public BaseJudger {
    bool submitted_;
public:
    OIJudger(size_t time_limit, size_t memory_limit, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer), submitted_(false) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        if (submitted_) {
            score_ = 0;
            return;
        }
        submitted_ = true;
        if (time <= time_limit_ && memory <= memory_limit_ && CheckAnswer(output)) {
            score_ = 100;
        } else {
            score_ = 0;
        }
    }

protected:
    bool CheckAnswer(const char *output) const override {
        const char *p_ans = answer_;
        const char *p_out = output;

        while (*p_ans != '\0' || *p_out != '\0') {
            const char *ans_next = strchr(p_ans, '\n');
            const char *out_next = strchr(p_out, '\n');

            const char *ans_line_end = ans_next ? ans_next : p_ans + strlen(p_ans);
            const char *out_line_end = out_next ? out_next : p_out + strlen(p_out);

            const char *ans_ptr = ans_line_end;
            while (ans_ptr > p_ans && *(ans_ptr - 1) == ' ') ans_ptr--;
            
            const char *out_ptr = out_line_end;
            while (out_ptr > p_out && *(out_ptr - 1) == ' ') out_ptr--;

            size_t ans_len = ans_ptr - p_ans;
            size_t out_len = out_ptr - p_out;

            if (ans_len != out_len || strncmp(p_ans, p_out, ans_len) != 0) {
                return false;
            }

            p_ans = ans_next ? ans_next + 1 : p_ans + strlen(p_ans);
            p_out = out_next ? out_next + 1 : p_out + strlen(p_out);
        }
        return true;
    }
};

class SpacialJudger : public BaseJudger {
    const size_t full_score_time_;
    const size_t full_score_memory_;
public:
    SpacialJudger(size_t time_limit, size_t memory_limit, size_t full_score_time, size_t full_score_memory, const char *answer)
        : BaseJudger(time_limit, memory_limit, answer), full_score_time_(full_score_time), full_score_memory_(full_score_memory) {}

    void Submit(size_t time, size_t memory, const char *output) override {
        if (!CheckAnswer(output)) return;

        size_t time_score = 0;
        if (time <= full_score_time_) time_score = 100;
        else if (time >= time_limit_) time_score = 0;
        else {
            time_score = 100 * (time_limit_ - time) / (time_limit_ - full_score_time_);
        }

        size_t memory_score = 0;
        if (memory <= full_score_memory_) memory_score = 100;
        else if (memory >= memory_limit_) memory_score = 0;
        else {
            memory_score = 100 * (memory_limit_ - memory) / (memory_limit_ - full_score_memory_);
        }

        size_t current_score = (time_score * memory_score) / 100;
        if (current_score > score_) score_ = current_score;
    }
};
