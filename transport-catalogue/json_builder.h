#pragma once

#include "json.h"

#include <optional>

namespace json {

    class DictKeyContext;
    class DictItemContext;
    class ArrayItemContext;

    class Builder {
    public:
        Builder();
        DictKeyContext Key(std::string key);
        Builder& Value(Node value);
        DictItemContext StartDict();
        Builder& EndDict();
        ArrayItemContext StartArray();
        Builder& EndArray();
        Node Build();
        Node GetNode(Node::Value value);

    private:
        Node root_{ nullptr };
        std::vector<Node*> nodes_stack_;
        template <typename Type>
        void InputResult(Type object) {
            if (nodes_stack_.back()->IsArray()) {
                const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(object);
                nodes_stack_.emplace_back(&const_cast<Array&>(nodes_stack_.back()->AsArray()).back());
            }
            else {
                *nodes_stack_.back() = object;
            }
        }
    };

    class DictItemContext : public Builder {
    public:
        DictItemContext(Builder& builder);

        DictKeyContext Key(std::string key);
        Builder& EndDict();
    private:
        Builder& builder_;
    };

    class ArrayItemContext : public Builder {
    public:
        ArrayItemContext(Builder& builder);

        ArrayItemContext Value(Node value);
        DictItemContext StartDict();
        Builder& EndArray();
        ArrayItemContext StartArray();
    private:
        Builder& builder_;
    };

    class DictKeyContext : public Builder {
    public:
        DictKeyContext(Builder& builder);

        DictItemContext Value(Node value);
        ArrayItemContext StartArray();
        DictItemContext StartDict();
    private:
        Builder& builder_;
    };

} // namespace json