#include "expr.h"

namespace sym 
{
	Expr::Expr(const Const& c){expr_ = c.eval(); }
	Expr::Expr(const Var& v)  {expr_ = v.eval(); }
	Expr::Expr(const __expr_t* e){expr_ = e; }
	Expr::Expr(const __expr_t& e) { expr_ = &e;}
	Expr::Expr(const Expr& e) {expr_ = e.eval();}
		
	Expr::~Expr() { 
	    delete expr_;
	}

	__expr_t* Expr::eval(const var_map_t& var_map) const {
	    return expr_->eval(var_map);
	}
	__expr_t* Expr::diff(const std::string& v) const {
	    __expr_t* ed = expr_->diff(v);
	    return ed;
	}
	std::ostream& Expr::operator<< (std::ostream &out) const { 
	    out << *expr_;
	    return out;
	}
	bool Expr::operator==(const Expr& other) const { 
	     return (*expr_) == (*other.expr_);
	}
	bool Expr::operator==(const __expr_t& other) const { 
	    return (*expr_) == other;
	}
}                                                                  	
