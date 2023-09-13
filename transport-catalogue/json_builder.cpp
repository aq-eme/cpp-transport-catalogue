#include "json_builder.h"

namespace json {

    Builder::Builder() {
        Node* root_ptr = &root_;
        nodes_stack_.emplace_back(root_ptr);
    }

    DictKeyContext Builder::Key(std::string key) {
        if (!(!nodes_stack_.empty() && nodes_stack_.back()->IsDict())) {
            throw std::logic_error("Wrong map key: " + key);
    }
        nodes_stack_.emplace_back(&const_cast<Dict&>(nodes_stack_.back()->AsDict())[key]);
        return *this;
    }

    Builder& Builder::Value(Node value) {
        if (nodes_stack_.empty() || (!nodes_stack_.back()->IsNull() && !nodes_stack_.back()->IsArray())) {
            throw std::logic_error("Value error");
        }
        if (nodes_stack_.back()->IsArray()) {
            const_cast<Array&>(nodes_stack_.back()->AsArray()).emplace_back(value);
        }
        else {
            *nodes_stack_.back() = value;
            nodes_stack_.pop_back();
        }
        return *this;
    }

    DictItemContext Builder::StartDict() {
        if (nodes_stack_.empty() || (!nodes_stack_.back()->IsNull() && !nodes_stack_.back()->IsArray())) {
            throw std::logic_error("StartDict error");
        }
        InputResult(Dict());
        return *this;
    }

    Builder& Builder::EndDict() {
        if (!(!nodes_stack_.empty() && nodes_stack_.back()->IsDict())) {
            throw std::logic_error("EndDict() called in wrong context");
        }
        nodes_stack_.pop_back();
        return *this;
    }

    ArrayItemContext Builder::StartArray() {
        if (nodes_stack_.empty() || (!nodes_stack_.back()->IsNull() && !nodes_stack_.back()->IsArray())) {
            throw std::logic_error("StartArray error");
        }
        InputResult(Array());
        return *this;
    }

    Builder& Builder::EndArray() {
        if (!(!nodes_stack_.empty() && nodes_stack_.back()->IsArray())) {
            throw std::logic_error("EndArray() called in wrong context");
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Node Builder::Build() {
        if (!nodes_stack_.empty()) {
            throw std::logic_error("Wrong Build()");
        }
        return root_;
    }

    Node Builder::GetNode(Node::Value value) {
        if (std::holds_alternative<int>(value)) return Node(std::get<int>(value));
        if (std::holds_alternative<double>(value)) return Node(std::get<double>(value));
        if (std::holds_alternative<std::string>(value)) return Node(std::get<std::string>(value));
        if (std::holds_alternative<std::nullptr_t>(value)) return Node(std::get<std::nullptr_t>(value));
        if (std::holds_alternative<bool>(value)) return Node(std::get<bool>(value));
        if (std::holds_alternative<Dict>(value)) return Node(std::get<Dict>(value));
        if (std::holds_alternative<Array>(value)) return Node(std::get<Array>(value));
        return {};
    }

    DictItemContext::DictItemContext(Builder& builder)
            : builder_(builder)
    {}

    DictKeyContext DictItemContext::Key(std::string key) {
        return builder_.Key(key);
    }

    Builder& DictItemContext::EndDict() {
        return builder_.EndDict();
    }

    ArrayItemContext::ArrayItemContext(Builder& builder)
            : builder_(builder)
    {}

    ArrayItemContext ArrayItemContext::Value(Node value) {
        return ArrayItemContext(builder_.Value(value));
    }

    DictItemContext ArrayItemContext::StartDict() {
        return builder_.StartDict();
    }

    ArrayItemContext ArrayItemContext::StartArray() {
        return builder_.StartArray();
    }

    Builder& ArrayItemContext::EndArray() {
        return builder_.EndArray();
    }

    DictKeyContext::DictKeyContext(Builder& builder)
            : builder_(builder)
    {}

    DictItemContext DictKeyContext::Value(Node value) {
        return DictItemContext(builder_.Value(value));
    }

    ArrayItemContext DictKeyContext::StartArray() {
        return builder_.StartArray();
    }

    DictItemContext DictKeyContext::StartDict() {
        return builder_.StartDict();
    }

} // namespace json