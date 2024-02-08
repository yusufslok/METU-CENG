module PE1 where

import Text.Printf

-- PE1: Recipe Calculator
-- The premise of this homework if to write a recipe calculator that
-- calculates: how much a recipe costs to make, what can be made with the
-- ingredients already available, and how much extra ingredients need to
-- be bought in order to make a recipe.

-- Recipe = Recipe Name [(Ingredient, Quantity)]
data Recipe = Recipe String [(String, Double)] deriving Show

-- Price = Price Ingredient Quantity Price
data Price = Price String Double Double deriving Show

-- You can use this as-is
getRounded :: Double -> Double 
getRounded x = read s :: Double
               where s = printf "%.2f" x

-- Calculate how much the given amount of the given ingredient costs
getIngredientCost :: (String, Double) -> [Price] -> Double
getIngredientCost (ingredientName, quantity) ((Price name quantity2 price):b) = if (ingredientName == name) 
                                                                                then getRounded (price * quantity / quantity2)
                                                                                else getIngredientCost (ingredientName, quantity) b


-- Calculate how much it costs to buy all the ingredients of a recipe
recipeCost :: Recipe -> [Price] -> Double
recipeCost (Recipe name []) price  = 0
recipeCost (Recipe name ((ingredientName, quantity):b)) price  =  getRounded(getRounded(getIngredientCost (ingredientName, quantity) price) + getRounded(recipeCost (Recipe name b) price))

-- Given a list of how much you already have of each ingredient,
-- calculate how much of which ingredients are missing for a recipe
missingIngredients :: Recipe -> [(String, Double)] -> [(String, Double)]
missingIngredients (Recipe name []) stock = []
missingIngredients (Recipe name ((ingredientName, quantity):b)) stock = if (quantity > getFromStock stock ingredientName) 
                                                                        then [(ingredientName, getRounded(quantity - getFromStock stock ingredientName))] ++ missingIngredients (Recipe name b) stock 
                                                                        else missingIngredients (Recipe name b) stock


getFromStock [] name = 0
getFromStock ((ingredientName, quantity):b) name = if (name == ingredientName) then quantity
    else getFromStock b name

-- Given a list of ingredients in your kitchen, calculate what you would
-- have left after making the given recipe. If there isn't enough of an
-- ingredient, the recipe cannot be made! You shouldn't change the amount
-- of ingredient in that case.
makeRecipe :: [(String, Double)] -> Recipe -> [(String, Double)]
makeRecipe stock recipe = if (isEnough stock recipe) then calculateRemains stock recipe else stock 

isEnough stock (Recipe name []) = True
isEnough stock (Recipe name ((ingredientName, quantity):b)) = if (isThere stock (ingredientName, quantity)) then isEnough stock (Recipe name b) else False

isThere [] (ingredientName, quantity) = False
isThere ((name, quantity2):b) (ingredientName, quantity) = if (ingredientName == name && quantity2 >= quantity) then True else if (ingredientName == name && quantity2 < quantity) then False else isThere b (ingredientName, quantity)
 
calculateRemains [] recipe = []
calculateRemains ((name, quantity2):b) recipe = calculateOneRemain (name, quantity2) recipe  ++ calculateRemains b recipe

calculateOneRemain (name, quantity2) (Recipe name1 []) = [(name, quantity2)]
calculateOneRemain (name, quantity2) (Recipe name1 ((ingredientName, quantity):b)) = if (ingredientName == name && not(quantity2 - quantity == 0)) 
                                                                                        then [(name, quantity2 - quantity)] 
                                                                                        else if (ingredientName == name && (quantity2 - quantity == 0))
                                                                                        then []
                                                                                        else calculateOneRemain (name, quantity2) (Recipe name1 b)

-- Given a list of ingredients you already have, and a list of recipes,
-- make a shopping list showing how much of each ingredient you need
-- to buy, and its cost. Each ingredient mush appear in the shopping list
-- at most once (no duplicates!).
makeShoppingList :: [(String, Double)] -> [Recipe] -> [Price] -> [(String, Double, Double)]
makeShoppingList stock recipes prices = createList stock prices recipes []

createList stock prices [] list = createShoppingList prices (missingIngredients (Recipe "a" list) stock) 
createList stock prices (Recipe name ([]):c) list = createList stock prices c list
createList stock prices (Recipe name1 ((ingredientName, quantity):b):c) list = if (not(isThereList list (ingredientName, quantity))) 
                                                                               then createList stock prices (Recipe name1 (b):c) ([(ingredientName, combineElement (Recipe name1 (b):c) (ingredientName, quantity))] ++ list)
                                                                               else createList stock prices (Recipe name1 (b):c) list 

                                                                                
isThereList [] (ingredientName, quantity) = False
isThereList ((name, quantity2):b) (ingredientName, quantity) = if (ingredientName == name) then True else isThereList b (ingredientName, quantity)



combineElement [] (ingredientName, quantity) = quantity
combineElement (Recipe name ([]):c) (ingredientName, quantity) = combineElement c (ingredientName, quantity)
combineElement (Recipe name1 ((name, quantity2):b):c) (ingredientName, quantity) = if (ingredientName == name) 
                                                                                    then combineElement ((Recipe name b):c) (ingredientName, quantity + quantity2) 
                                                                                    else combineElement ((Recipe name b):c) (ingredientName, quantity)

createShoppingList prices [] = []
createShoppingList prices ((ingredientName, quantity):b) =  [(ingredientName, quantity, recipeCost (Recipe "name" [(ingredientName, quantity)]) prices)] ++ createShoppingList prices b