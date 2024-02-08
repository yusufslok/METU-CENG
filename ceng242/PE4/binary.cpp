#include "binary.h"
#include "nullary.h"
#include <math.h>

namespace sym 
{
	bool AddOp::is_add() const {return true;}

	__expr_t* AddOp::eval(const var_map_t& vars) const { 
        __expr_t* lhsEval = lhs_->eval(vars);
        __expr_t* rhsEval = rhs_->eval(vars);
        if(dynamic_cast<Const*>(rhsEval) && dynamic_cast<Const*>(lhsEval)){
            double result = dynamic_cast<Const*>(lhsEval)->get_value()+dynamic_cast<Const*>(rhsEval)->get_value();
            delete lhsEval;delete rhsEval;
            return new Const(result);
        }
        else if(dynamic_cast<Const*>(rhsEval) && dynamic_cast<Const*>(rhsEval)->get_value() == 0.0){
            delete rhsEval;
            return lhsEval;
        }
        else if(dynamic_cast<Const*>(lhsEval) && dynamic_cast<Const*>(lhsEval)->get_value() == 0.0){
            delete lhsEval;
            return rhsEval;
        }
        else
            return new AddOp(lhsEval,rhsEval);
	}

	__expr_t* AddOp::diff(const std::string& v) const {
	    __expr_t* op1 = lhs_->diff(v);
	    __expr_t* op2 = rhs_->diff(v);
	    return AddOp(op1,op2).eval();
	    
	}

	std::ostream& AddOp::operator<< (std::ostream &out) const {
	    if((lhs_->is_nullary()) == false){
	        out << "(" ;
	        out << *lhs_;
	        out << ")";
	    }
	    else 
	        out << *lhs_;
	   out << " + ";
	    if((rhs_->is_nullary() == false)){
	        out << "(" ;
	        out << *rhs_;
	        out << ")";
	    }
	    else
	        out << *rhs_;
	    return out;
	}

	bool AddOp::operator==(const __expr_t& other_) const {
	    const AddOp* addOperand = dynamic_cast< const AddOp*>(&other_);
	    if(addOperand){
	        if(*lhs_ == *(addOperand -> lhs_) && *rhs_ == *(addOperand -> rhs_))
	            return true;
	        else if(*rhs_ == *(addOperand -> lhs_) && *lhs_ == *(addOperand -> rhs_))
	            return true;
	    }
	    return false;
	    
	}
}

namespace sym 
{
	bool MulOp::is_mul() const { return true; }

	__expr_t* MulOp::eval(const var_map_t& vars) const { 
	    __expr_t* eval1 = lhs_->eval(vars);
	    __expr_t* eval2 = rhs_->eval(vars);
	    if(dynamic_cast<Const*>(eval1) && dynamic_cast<Const*>(eval1)->get_value() == 0.0 || dynamic_cast<Const*>(eval2) && dynamic_cast<Const*>(eval2)->get_value() == 0.0){
	        delete eval1;delete eval2;
	        return new Const(0.0);
	    }
	    else if(dynamic_cast<Const*>(eval1) && dynamic_cast<Const*>(eval1)->get_value() == 1.0){
	        delete eval1;
	        return eval2;
	    }
	    else if(dynamic_cast<Const*>(eval2) && dynamic_cast<Const*>(eval2)->get_value() == 1.0){
	        delete eval2;
	        return eval1;
	    }
	    else if(dynamic_cast<Const*>(eval2) && dynamic_cast<Const*>(eval1)){
	        return new Const(dynamic_cast<Const*>(eval1)->get_value()*dynamic_cast<Const*>(eval2)->get_value());
	    }
	    else{
	        return new MulOp(eval1,eval2);
	    }
	}

	__expr_t* MulOp::diff(const std::string& v) const {
	    __expr_t* diff1 = lhs_->diff(v);
	    __expr_t* diff2 = rhs_->diff(v);
	    return AddOp(MulOp(diff1,rhs_->eval()).eval(), MulOp(lhs_->eval(),diff2).eval()).eval();
	}

	std::ostream& MulOp::operator<< (std::ostream &out) const {
	    
	    if((lhs_->is_nullary()) == false){
	        out << "(" ;
	        lhs_->operator<<(out);
	        out << ")";
	    }
	    else 
	        lhs_->operator<<(out);
	   out << " * ";
	    if((rhs_->is_nullary() == false)){
	        out << "(" ;
	        rhs_->operator<<(out);
	        out << ")";
	    }
	    else
	        rhs_->operator<<(out);
	    return out;
	}

	bool MulOp::operator==(const __expr_t& other_) const { 
	    const MulOp* mulop = dynamic_cast<const MulOp*>(&other_);
	    if(mulop){
	        if(*lhs_ == *(mulop -> lhs_) && *rhs_ == *(mulop -> rhs_))
	            return true;
	        else if(*rhs_ == *(mulop -> lhs_) && *lhs_ == *(mulop -> rhs_))
	            return true;
	    }
	    return false;
	}
}
