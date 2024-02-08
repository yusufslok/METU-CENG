#include "nullary.h"
#include "unary.h"
#include "binary.h"
#include <math.h>

namespace sym 
{
	bool NegOp::is_neg() const { 
	   return true;
	}

	__expr_t* NegOp::eval(const var_map_t& vars) const {
	    __expr_t* op = operand->eval(vars);
	    if(dynamic_cast<Const*>(op)){
	        double v = dynamic_cast<Const*>(op)->get_value();
	        delete op;
	        return new Const(-1*v);
	    }
        return new NegOp(op);
	    
	}

	__expr_t* NegOp::diff(const std::string& v) const {
	    __expr_t* diffop = operand->diff(v);
        return NegOp(diffop).eval();
	    
	}

	std::ostream& NegOp::operator<< (std::ostream &out) const {
	    if(operand->is_nullary() == false){
	    out << "-(";
        out << *operand;
	    out << ")";
	    }
	    else {out << "-" << *operand;}
        return out;
	}

	bool NegOp::operator==(const __expr_t& other_) const {
	     const NegOp* negOp = dynamic_cast<const NegOp*>(&other_);
        if (negOp) {
            return *operand == *(negOp->operand);
        }
        return false;
	    
	}
}

namespace sym 
{
	bool ExpOp::is_exp() const {
	    return true;}

	__expr_t* ExpOp::eval(const var_map_t& vars) const { 
	    __expr_t* op = operand->eval(vars);
	    if(dynamic_cast<Const*>(op)){
	        double v = dynamic_cast<Const*>(op)->get_value();
	        delete op;
	        return new Const(std::exp(v));
	    }
	    return new ExpOp(op);
	}

	__expr_t* ExpOp::diff(const std::string& v) const {
	    __expr_t* diffop = operand->diff(v);
	    return MulOp(diffop, new ExpOp(operand->eval())).eval();
	}

	std::ostream& ExpOp::operator<< (std::ostream &out) const { 
	    bool nullaryOp = dynamic_cast<const __nullary_op_t*>(operand) != nullptr;
        out << "e^";
        if (!nullaryOp)
            out << "(";
        out << *operand;
        if (!nullaryOp)
            out << ")";

        return out;
	}

	bool ExpOp::operator==(const __expr_t& other_) const {
	    const ExpOp* otherExpOp = dynamic_cast<const ExpOp*>(&other_);
        if (otherExpOp != nullptr) {
            return *operand == *(otherExpOp->operand);
        }
        return false;
	    
	}
}
