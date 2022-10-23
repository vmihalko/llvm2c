#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IntrinsicInst.h>

Type *fixType(Program& program, const llvm::DIType *ditype) {
        // Int or float
        if (auto tbasic = llvm::dyn_cast_or_null<llvm::DIBasicType>(ditype)) {
            if (!tbasic->getSignedness().hasValue()) {
                if (tbasic->getEncoding() == llvm::dwarf::DW_ATE_float) {
                    if (tbasic->getSizeInBits() <= 32)
                        return program.typeHandler.floatType.get();
                    if (tbasic->getSizeInBits() <= 64)
                        return program.typeHandler.doubleType.get();
                    return program.typeHandler.longDoubleType.get();
                }
            }

            bool signedness = llvm::DIBasicType::Signedness::Signed == *tbasic->getSignedness();
            if (tbasic->getSizeInBits() == 1) {
                return (signedness ? program.typeHandler.sint.get() : program.typeHandler.uint.get());
            }
            if (tbasic->getSizeInBits() <= 8) {
                return (signedness ? program.typeHandler.schar.get() : program.typeHandler.uchar.get());
            }
            if (tbasic->getSizeInBits() <= 16) {
                return (signedness ? program.typeHandler.sshort.get() : program.typeHandler.ushort.get());
            }
            if (tbasic->getSizeInBits() <= 32) {
                return (signedness ? program.typeHandler.sint.get() : program.typeHandler.uint.get());
            }
            if (tbasic->getSizeInBits() <= 64) {
                return (signedness ? program.typeHandler.slong.get() : program.typeHandler.ulong.get());
            }
        }
        // what is llvm::DICompositeTypeArray ?
        // if ( (const llvm::DICompositeType* diCompType = llvm::dyn_cast<llvm::DICompositeType>(ditype)) &&
        //                                             llvm::dwarf::DW_TAG_array_type == diCompType->getTag() ) {
        //     auto ty = std::make_unique<ArrayType>(fixType(program, diCompType->getBaseType()) ,
        //                                                     diCompType->getElements().size());
        //     //auto result = ty.get();
        //     //auto* inner = getType(PT->getPointerElementType());
        //     //makeCachedType<PointerType>(PT, inner)
        //     // program.typeHandler.makeCachedType<ArrayType>(type, fixType(program, diCompType->getBaseType()),
        //     //                                                         diCompType->getElements().size());
        //     // TODO Insert to typeCache.insert(it, std::make_pair(type, std::move(ty)));
        // }
        const llvm::DICompositeType* diCompType = llvm::dyn_cast<llvm::DICompositeType>(ditype);
        if ( diCompType && llvm::dwarf::DW_TAG_array_type == diCompType->getTag() ) {
            // WRONG NUMBER OF ELEMENTS
            uint32_t numberOfArrElem = 1;

            // https://llvm.org/doxygen/BPFAbstractMemberAccess_8cpp_source.html#l00334
            // why this heuristic?
            if (auto *Element = llvm::dyn_cast_or_null<llvm::DINode>(diCompType->getElements()[0])) {
                if (Element->getTag() == llvm::dwarf::DW_TAG_subrange_type) {
                    const llvm::DISubrange *SR = llvm::cast<llvm::DISubrange>(Element);
                    auto *CI = SR->getCount().dyn_cast<llvm::ConstantInt *>();
                    numberOfArrElem = CI->getSExtValue();
            return program.typeHandler.cachedDITypeInserter<ArrayType>(ditype, fixType(program, diCompType->getBaseType()),
                                                                 numberOfArrElem);
                }
            }
        }
        if( const llvm::DIDerivedType* diDerivedType = llvm::dyn_cast<llvm::DIDerivedType>(ditype)) {
            return fixType(program, diDerivedType->getBaseType());
        }
        return nullptr;
}

static void setMetadataInfo(Program& program, const llvm::CallInst* ins, Block* block) {
    llvm::Metadata* md = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(0))->getMetadata();
    llvm::Value* referredVal = llvm::cast<llvm::ValueAsMetadata>(md)->getValue();
    Expr* referred = block->func->getExpr(referredVal);

    if (auto* re = llvm::dyn_cast_or_null<RefExpr>(referred)) {
        referred = re->expr;
    }

    if (Value* variable = llvm::dyn_cast_or_null<Value>(referred)) {
        llvm::Metadata* varMD = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(1))->getMetadata();
        llvm::DILocalVariable* localVar = llvm::dyn_cast_or_null<llvm::DILocalVariable>(varMD);

        if (auto t = fixType(program, localVar->getType()))
            variable->setType(t);
    }
}

void parseMetadataTypes(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateAllocas));

    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                if (ins.getOpcode() == llvm::Instruction::Call) {
                    const llvm::CallInst* CI = llvm::cast<llvm::CallInst>(&ins);
                    //CI->getIntrinsicID() ===
                    if (CI->getCalledFunction()) {
                        if (CI->getIntrinsicID() == llvm::Intrinsic::dbg_declare) {
                            setMetadataInfo(program, CI, myBlock);
                        }
                    }
                }
            }

        }
    }
    program.addPass(PassType::ParseMetadataTypes);
}
