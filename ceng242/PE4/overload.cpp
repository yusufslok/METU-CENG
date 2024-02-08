#include "nullary.h"
#include "unary.h"
#include "binary.h"

namespace sym {
	__expr_t& operator-(const __expr_t &op) { 
	    if(op.is_nullary()){
	        return *(new NegOp(op.eval()));
	    }
	    return *(new NegOp(&op));}
	__expr_t& exp(const __expr_t &op) { 
	    if(op.is_nullary()){
	        return *(new ExpOp(op.eval()));
	    }return *(new ExpOp(&op));}

	__expr_t& operator+(const __expr_t &lhs, const __expr_t &rhs) {
	    if(lhs.is_nullary() && rhs.is_nullary()){
	        return *(new AddOp(lhs.eval(), rhs.eval()));
	    }
	    else if(lhs.is_nullary()){
	        return *(new AddOp(lhs.eval(), &rhs));
	    }
	    else if(rhs.is_nullary()){
	        return *(new AddOp(&lhs, rhs.eval()));
	    }
	    return *(new AddOp(&lhs, &rhs)); }
	    
	__expr_t& operator+(double lhs, const __expr_t &rhs) {  
	    Const* c = new Const(lhs);
	    if(rhs.is_nullary()){
	        return *(new AddOp(c, rhs.eval()));
	    }
        return *(new AddOp(c, &rhs));}
        
	__expr_t& operator+(const __expr_t &lhs, double rhs) {
	    Const* c = new Const(rhs);
	    if(lhs.is_nullary()){
	        return *(new AddOp(lhs.eval(), c));
	    }
        return *(new AddOp(&lhs, c));
	}

	__expr_t& operator*(const __expr_t &lhs, const __expr_t &rhs) {
	    if(lhs.is_nullary() && rhs.is_nullary()){
	        return *(new MulOp(lhs.eval(), rhs.eval())); 
	    }
	    else if(lhs.is_nullary()){
	        return *(new MulOp(lhs.eval(), &rhs));
	    }
	    else if(rhs.is_nullary()){
	        return *(new MulOp(&lhs, rhs.eval()));
	    }
	    return *(new MulOp(&lhs, &rhs)); }
	__expr_t& operator*(double lhs, const __expr_t &rhs) {
	     Const* c =  new Const(lhs);
	     if(rhs.is_nullary()){
	        return *(new MulOp(c, rhs.eval()));
	    }
        return *(new MulOp(c, &rhs));}
	__expr_t& operator*(const __expr_t &lhs, double rhs) {
	    Const* c =  new Const(rhs);
	    if(lhs.is_nullary()){
	        return *(new MulOp(lhs.eval(), c));
	    }
        return *(new MulOp(&lhs, c));
	}
}
