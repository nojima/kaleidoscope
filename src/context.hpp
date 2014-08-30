#pragma once

#include <map>
#include <string>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "position.hpp"

namespace kaleidoscope {
    class Context {
    public:
        Context(llvm::Module* module, llvm::LLVMContext& llvmContext):
            module_(module), builder_(llvmContext) {}

        llvm::Module* module() {
            return module_;
        }

        llvm::IRBuilder<>& builder() {
            return builder_;
        }

        std::map<std::string, llvm::Value*>& namedValues() {
            return namedValues_;
        }

    private:
        llvm::Module* module_;
        llvm::IRBuilder<> builder_;
        std::map<std::string, llvm::Value*> namedValues_;
    };
};
