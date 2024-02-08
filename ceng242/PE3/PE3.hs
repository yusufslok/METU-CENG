{-# LANGUAGE FlexibleInstances #-}

module PE3 where


import Data.List (sort, sortBy)
import Text.Printf (printf)

data Term = Const Integer | Pw Integer Power | Trig Integer Power Trigonometric | Exp Integer Power Exponential

data Power = Power Integer
data Polynomial = Polynomial [(Integer, Power)]
data Exponential = Exponential Polynomial
data Trigonometric = Sin Polynomial | Cos Polynomial

class Evaluable a where
    function :: a -> (Integer -> Double)

class Differentiable a where
    derivative :: a -> [Term]

-- You can use this as is
getRounded :: Double -> Double 
getRounded x = read s :: Double
               where s = printf "%.2f" x

-- You don't have to follow the order the functions appear in the file
-- For example, you could first define all Show instances, then all Eq instances etc.
-- if that implementation order is more convenient for you.



-- INSTANCES FOR POWER

instance Show Power where
    show (Power x)
        | x == 0 = "1"
        | x == 1 = "x"
        | otherwise = "x^" ++ show x

  

instance Eq Power where
    (Power x) == (Power y) = x == y

instance Ord Power where
    (Power x) < (Power y) = x < y
    (Power x) <= (Power y) = x <= y
    (Power x) > (Power y) = x > y
    (Power x) >= (Power y) = x >= y

instance Evaluable Power where
    function (Power p) = \x -> (fromInteger x)^p

instance Differentiable Power where
    derivative (Power p) = if (p == 0) then []
                            else if (p==1) then  [Const p]
                            else [Pw p (Power (p-1))]



-- INSTANCES FOR POLYNOMIAL

instance Show Polynomial where
     show (Polynomial ((int,p):xs))
        | int == 0 = rest
        | show (p) == "1" = show int ++ rest
        | int == 1 = show p ++ rest
        | int == -1 = "-" ++ show p ++ rest
        | otherwise = show int ++ show (p) ++ rest
        where
        rest = if (xs == []) then "" else " + " ++ show (Polynomial (xs))
        
instance Evaluable Polynomial where
  function (Polynomial []) = \x -> 0.0
  function (Polynomial ((int,p):xs)) = \x -> fromInteger int * function p x + function (Polynomial xs) x



instance Differentiable Polynomial where
    derivative (Polynomial []) = []
    derivative (Polynomial ((int,Power p):xs)) = if(int /= 0 && p /= 0)
                                                    then [Pw (int*p) (Power (p-1))] ++ derivative (Polynomial (xs))
                                                    else derivative (Polynomial (xs))




-- INSTANCES FOR TRIGONOMETRIC

instance Show Trigonometric where
    show (Sin p)
        | helper p = "sin(" ++ show p ++ ")"
        | otherwise = "sin" ++ show p
    show (Cos p)
        | helper p = "cos(" ++ show p ++ ")"
        | otherwise = "cos" ++ show p
    

instance Evaluable Trigonometric where
    function (Sin p) = \x -> getRounded (sin(function p x))
    function (Cos p) = \x -> getRounded (cos(function p x))

instance Differentiable Trigonometric where
    derivative a = derivativest a a 
    
derivativest (Sin (Polynomial [])) a = []
derivativest (Sin (Polynomial ((int,Power p):xs))) (Sin (Polynomial ((s,Power ss):sss))) = if(p == 0 || int == 0)
                                                        then derivativest (Sin (Polynomial (xs))) (Sin (Polynomial ((s,Power ss):sss)))
                                                        else [Trig (p*int) (Power (p-1)) (Cos (Polynomial ((s,Power ss):sss)))] ++ derivativest (Sin (Polynomial (xs))) (Sin (Polynomial ((s,Power ss):sss)))

derivativest (Cos (Polynomial [])) a = []
derivativest (Cos (Polynomial ((int,Power p):xs))) (Cos (Polynomial ((s,Power ss):sss))) = if(p == 0 || int == 0)
                                                        then derivativest (Cos (Polynomial (xs))) (Cos (Polynomial ((s,Power ss):sss)))
                                                        else [Trig ((-1)*p*int) (Power (p-1)) (Sin (Polynomial ((s,Power ss):sss)))] ++ derivativest (Cos (Polynomial (xs))) (Cos (Polynomial ((s,Power ss):sss)))



-- INSTANCES FOR EXPONENTIAL

instance Show Exponential where
    show (Exponential p) 
        | helper p = "e^(" ++ show p ++ ")"
        | otherwise = "e^" ++ show p
        
helper (Polynomial ((int,Power x):xs)) = if (((x == 0) || x == 1) && a && int > 0) then False
                                            else True
                                            where
                                            a = if ((xs == [])) then True else False  

instance Evaluable Exponential where
  function (Exponential p) = \x -> getRounded (exp (function p x))

instance Differentiable Exponential where
    
    derivative a = derivatives a a
    
derivatives (Exponential (Polynomial [])) a = []
derivatives (Exponential (Polynomial ((int,Power p):xs))) a = if(int == 0 || p == 0) 
                                                                then derivatives (Exponential (Polynomial (xs))) a
                                                                else [Exp (p*int) (Power (p-1)) a] ++ derivatives (Exponential (Polynomial (xs))) a


-- INSTANCES FOR TERM

instance Show Term where
    show (Const x) = show x
    show (Pw x p)
        | show p == "1" = show x
        | x == 1 = show p
        | x == -1 = "-" ++ show p
        | otherwise = show x ++ show p
    show (Trig x p t)
        | (show p == "1" && x == 1) = show t
        | (show p == "1" && x == -1) = "-" ++ show t
        | show p == "1" = show x ++ show t
        | x == 1 = show p ++ show t
        | x == -1 = "-" ++ show p ++ show t
        | otherwise = show x ++ show p ++ show t
    show (Exp x p ex)
        | (show p == "1" && x == 1) = show ex
        | (show p == "1" && x == -1) = "-" ++ show ex
        | show p == "1" = show x ++ show ex
        | x == 1 = show p ++ show ex
        | x == -1 = "-" ++ show p ++ show ex
        | otherwise = show x ++ show p ++ show ex

instance Evaluable Term where
    function (Const a) = \x -> fromInteger a
    function (Pw a p) = \x -> fromInteger a * function p x
    function (Trig a p t) = \x -> fromInteger a * function p x * function t x
    function (Exp a p ex) = \x -> fromInteger a * function p x * function ex x
    

instance Differentiable Term where
    derivative (Const a) = []
    derivative (Pw a (Power p)) = if (a == 0 || p == 0) then [] else [Pw (a*p) (Power (p-1))]
    
    derivative (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) = derivativesterm (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) (Sin (Polynomial ((int,Power p):xs)))
    derivative (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) = derivativesterm (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) (Cos (Polynomial ((int,Power p):xs)))
     
    derivative (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) = derivatexp (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) (Exponential (Polynomial ((int,Power p):xs)))
    
derivativesterm (Trig a (Power pp) (Sin (Polynomial []))) (Sin (Polynomial ((inttt,Power ppp):sss))) = if (pp /= 0) then [Trig (a*pp) (Power (pp-1)) (Sin (Polynomial ((inttt,Power ppp):sss)))]
                                                                                                        else []
derivativesterm (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) (Sin (Polynomial ((inttt,Power ppp):sss))) = if (int == 0 || p == 0) then derivativesterm (Trig a (Power pp) (Sin (Polynomial (xs)))) (Sin (Polynomial ((inttt,Power ppp):sss)))
                                                                                else [Trig (a*int*p) (Power (pp+p-1)) (Cos (Polynomial ((inttt,Power ppp):sss)))] ++ derivativesterm (Trig a (Power pp) (Sin (Polynomial (xs)))) (Sin (Polynomial ((inttt,Power ppp):sss)))

derivativesterm (Trig a (Power pp) (Cos (Polynomial []))) (Cos (Polynomial ((inttt,Power ppp):sss))) = if (pp /= 0) then [Trig (a*pp) (Power (pp-1)) (Cos (Polynomial ((inttt,Power ppp):sss)))]
                                                                                                        else []
derivativesterm (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) (Cos (Polynomial ((inttt,Power ppp):sss))) = if (int == 0 || p == 0) then derivativesterm (Trig a (Power pp) (Cos (Polynomial (xs)))) (Cos (Polynomial ((inttt,Power ppp):sss)))
                                                                                                                        else [Trig (-1*a*int*p) (Power ((pp+p-1))) (Sin (Polynomial ((inttt,Power ppp):sss)))] ++ derivativesterm (Trig a (Power pp) (Cos (Polynomial (xs)))) (Cos (Polynomial ((inttt,Power ppp):sss)))
    
    
    
derivatexp (Exp a (Power pp) (Exponential (Polynomial []))) aa = if (pp /= 0) then [Exp (a*pp) (Power (pp-1)) aa]
                                                                    else []
derivatexp (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) aa = if (int == 0 || p == 0 ) then derivatexp (Exp a (Power pp) (Exponential (Polynomial (xs)))) aa
                                                                                else [Exp (int*p*a) (Power (pp+p-1)) aa] ++ derivatexp (Exp a (Power pp) (Exponential (Polynomial (xs)))) aa
-- INSTANCES FOR [TERM]

instance Evaluable [Term] where
    function [] = \x -> 0.0
    function (a:b) = \x -> getRounded (function1 (a:b) x)
    
function1 [] = \x -> 0.0
function1 (a:b) = \x -> function a x + function1 b x
    

instance Differentiable [Term] where
    derivative list = merge (derivateList list) []

merge [] list = []
merge (a:b) list = if (not(isThere a list)) then (helperr a b) ++ merge b (list++[a])
                    else merge b (list)

isThere (Pw a (Power p)) [] = False
isThere (Pw a (Power p)) ((Pw aa (Power pp)):b) = if (p == pp) then True 
                                                    else isThere (Pw a (Power p)) (b)
isThere (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) [] = False                                                    
isThere (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) ((Trig aa (Power ppp) (Cos (Polynomial ((intt,Power pppp):xss)))):b) = if (isItSamePoly (Polynomial ((intt,Power pppp):xss)) (Polynomial ((int,Power p):xs)) && pp == ppp) then True
                                                                                                                                            else isThere (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) (b)

isThere (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) [] = False                                                    
isThere (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) ((Trig aa (Power ppp) (Sin (Polynomial ((intt,Power pppp):xss)))):b) = if (isItSamePoly (Polynomial ((intt,Power pppp):xss)) (Polynomial ((int,Power p):xs)) && pp == ppp) then True
                                                                                                                                            else isThere (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) (b)
isThere (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) [] = False                                                                                                                                                                                                                                                                                     
isThere (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) (Exp aa (Power ppp) (Exponential (Polynomial ((intt,Power pppp):xss))):b) = if (isItSamePoly (Polynomial ((intt,Power pppp):xss)) (Polynomial ((int,Power p):xs)) && pp == ppp) 
                                                                                                                                                        then True
                                                                                                                                                        else isThere (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) (b)
isThere x (a:b) = isThere x b

helperr (Pw a (Power p)) [] = if (a == 0) then []
                                else [(Pw a (Power p))]
helperr (Pw a (Power p)) ((Pw aa (Power pp)):b) = if (pp == p) then helperr (Pw (a+aa) (Power p)) (b)
                                                    else helperr (Pw a (Power p)) (b) 

helperr (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) [] = if (a == 0) then []
                                                                        else [(Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs))))]
helperr (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) ((Trig aa (Power ppp) (Cos (Polynomial ((intt,Power pppp):xss)))):b) = if (isItSamePoly (Polynomial ((int,Power p):xs)) (Polynomial ((intt,Power pppp):xss)) && pp == ppp) 
                                                                                                                                            then helperr (Trig (a+aa) (Power pp) (Cos (Polynomial ((int,Power p):xs)))) (b)
                                                                                                                                            else helperr (Trig a (Power pp) (Cos (Polynomial ((int,Power p):xs)))) (b)
                                                                                                                                    
helperr (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) [] = if (a == 0) then []
                                                                        else [(Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs))))]
helperr (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) ((Trig aa (Power ppp) (Sin (Polynomial ((intt,Power pppp):xss)))):b) = if (isItSamePoly (Polynomial ((int,Power p):xs)) (Polynomial ((intt,Power pppp):xss)) && pp == ppp) 
                                                                                                                                                then helperr (Trig (a+aa) (Power pp) (Sin (Polynomial ((int,Power p):xs)))) (b)
                                                                                                                                                else helperr (Trig a (Power pp) (Sin (Polynomial ((int,Power p):xs)))) (b)
helperr (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) [] = if (a==0) then []
                                                                                else [(Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs))))]
helperr (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) (Exp aa (Power ppp) (Exponential (Polynomial ((intt,Power pppp):xss))):b) = if (isItSamePoly (Polynomial ((int,Power p):xs)) (Polynomial ((intt,Power pppp):xss)) && pp == ppp)
                                                                                                                                                    then helperr (Exp (a+aa) (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) (b)
                                                                                                                                                    else helperr (Exp a (Power pp) (Exponential (Polynomial ((int,Power p):xs)))) (b)
helperr x (a:b) = helperr x b
                                                                                                                                                    

isItSamePoly (Polynomial []) (Polynomial ([])) = True
isItSamePoly (Polynomial ((int,Power p):xs)) (Polynomial ((intt,Power pp):xss))  = if (p == pp && int == intt) 
                                                                                    then isItSamePoly (Polynomial (xs)) (Polynomial (xss))
                                                                                    else False


derivateList [] = []
derivateList (a:b) = derivative a ++ derivateList b
