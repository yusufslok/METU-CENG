module PE2 where

-- PE2: Dungeon Crawler
-- Dungeon map is :: Tree Chamber [Encounter]
-- Each encounter is either a fight or a treasure
-- Fights deal you damage (reduce HP) but enemies drop some gold (add
-- gold)
-- Tresures just give gold, or potions (which give hp)
-- Nodes hold encounters, when you visit a node you go through all of them in order
-- You start with a certain amount of HP and 0 gold.
-- You lose HP and accumulate gold as you descend the tree and go through encounters

-- Polymorphic tree structure
data Tree a b = EmptyTree | Leaf a b | Node a b [Tree a b] deriving (Show, Eq)

-- Every location in the tree is of some Chamber type.
data Chamber = Cavern |
               NarrowPassage |
               UndergroundRiver |
               SlipperyRocks deriving (Show, Eq)

-- An enemy has a name, an amount of damage that it deals
-- and an amount of gold that it drops (in that order).
data Enemy = Enemy String Integer Integer deriving (Show, Eq)

-- Gold n gives n amount of gold
-- Potion n heals n hp
data Loot = Gold Integer | Potion Integer deriving (Show, Eq)

-- An encounter is either a Fight with an Enemy, or a treasure where
-- you find Loot
data Encounter = Fight Enemy | Treasure Loot deriving (Show, Eq)

-- This is a type synonym for how we will represents our dungeons
type Dungeon = Tree Chamber [Encounter]

-- First argument is starting HP
-- Second argument is the dungeon map
-- Third argument is the path (each integer in the list shows what child
-- you descend into)
-- Calculate how much HP you have left and how much gold you've
-- accumulated after traversing the given path
traversePath :: Integer -> Dungeon -> [Int] -> (Integer, Integer)

traversePath hp map list = traversePathHelper hp map list (hp,0)

traversePathHelper hp (Leaf chamber []) [] (x,y) = (x,y)
traversePathHelper hp (Leaf chamber (Treasure (Gold value):e2)) indexList (x,y)= traversePathHelper hp (Leaf chamber (e2)) indexList (x,y+value)
traversePathHelper hp (Leaf chamber (Treasure (Potion value):e2)) indexList (x,y)= traversePathHelper hp (Leaf chamber (e2)) indexList (x+value,y)
traversePathHelper hp (Leaf chamber (Fight (Enemy _ damage gold):e2)) indexList (x,y) = traversePathHelper hp (Leaf chamber (e2) ) indexList (x-damage,y+gold)

traversePathHelper hp (Node chamber [] childList) [] (x,y) = (x,y)
traversePathHelper hp (Node chamber [] childList) (a:b) (x,y) = traversePathHelper hp (childList !! a) (b) (x,y)
traversePathHelper hp (Node chamber (Treasure (Gold value):e2) childList) indexList (x,y)= traversePathHelper hp (Node chamber (e2) childList) indexList (x,y+value)
traversePathHelper hp (Node chamber (Treasure (Potion value):e2) childList) indexList (x,y)= traversePathHelper hp (Node chamber (e2) childList) indexList (x+value,y)
traversePathHelper hp (Node chamber (Fight (Enemy _ damage gold):e2) childList) indexList (x,y) = traversePathHelper hp (Node chamber (e2) childList) indexList (x-damage,y+gold)


-- First argument is starting HP
-- Second argument is dungeon map
-- Find which path down the tree yields the most gold for you
-- You cannot turn back, i.e. you'll find a non-branching path
-- You do not need to reach the bottom of the tree
-- Return how much gold you've accumulated
findMaximumGain :: Integer -> Dungeon -> Integer
findMaximumGain hp dungeon = findMaximumGainHelper hp dungeon 0 0


findMaximumGainHelper hp (Leaf chamber a) maxGold curGold = if (x > 0 && curGold + y > maxGold) 
                                                                            then curGold + y
                                                                            else if (x > 0)
                                                                            then maxGold
                                                                            else maxGold
                                                                            where (x,y) = traversePathHelper1 hp (Leaf chamber a) [] (hp,0)

findMaximumGainHelper hp (Node chamber a []) maxGold curGold = maxGold
findMaximumGainHelper hp (Node chamber a childList) maxGold curGold = if (x > 0 && curGold + y > maxGold) 
                                                                            then maximum [findMaximumGainHelper x a (curGold+y) (curGold+y) | a<-childList]
                                                                            else if (x > 0)
                                                                            then maximum [findMaximumGainHelper x a maxGold (curGold+y) | a<-childList]
                                                                            else maxGold
                                                                            where (x,y) = traversePathHelper1 hp (Node chamber a childList) [] (hp,0)

traversePathHelper1 hp (Leaf chamber []) [] (x,y) = (x,y)
traversePathHelper1 hp (Leaf chamber (Treasure (Gold value):e2)) indexList (x,y)= traversePathHelper1 hp (Leaf chamber (e2)) indexList (x,y+value)
traversePathHelper1 hp (Leaf chamber (Treasure (Potion value):e2)) indexList (x,y)= traversePathHelper1 hp (Leaf chamber (e2)) indexList (x+value,y)
traversePathHelper1 hp (Leaf chamber (Fight (Enemy _ damage gold):e2)) indexList (x,y) = if (x-damage <= 0) 
                                                                                            then traversePathHelper1 hp (Leaf chamber [] ) [] (-1,y+gold)
                                                                                            else traversePathHelper1 hp (Leaf chamber (e2) ) indexList (x-damage,y+gold)

traversePathHelper1 hp (Node chamber [] childList) [] (x,y) = (x,y)
traversePathHelper1 hp (Node chamber [] childList) (a:b) (x,y) = traversePathHelper1 hp (childList !! a) (b) (x,y)
traversePathHelper1 hp (Node chamber (Treasure (Gold value):e2) childList) indexList (x,y)= traversePathHelper1 hp (Node chamber (e2) childList) indexList (x,y+value)
traversePathHelper1 hp (Node chamber (Treasure (Potion value):e2) childList) indexList (x,y)= traversePathHelper1 hp (Node chamber (e2) childList) indexList (x+value,y)
traversePathHelper1 hp (Node chamber (Fight (Enemy _ damage gold):e2) childList) indexList (x,y) = if (x-damage <= 0) 
                                                                                                    then traversePathHelper1 hp (Node chamber [] childList) [] (-1,y+gold)
                                                                                                    else traversePathHelper1 hp (Node chamber (e2) childList) indexList (x-damage,y+gold)



-- First argument is starting HP
-- Second argument is the dungeon map
-- Remove paths that you cannot go thorugh with your starting HP. (By
-- removing nodes from tree).
-- Some internal nodes may become leafs during this process, make the
-- necessary changes in such a case.
findViablePaths :: Integer -> Dungeon -> Dungeon
findViablePaths hp dungeon = findViablePathsHelper (traversePathHelper1 hp dungeon [] (hp,0)) dungeon


findViablePathsHelper (hp,gold) (Leaf chamber a) = Leaf chamber a
findViablePathsHelper (hp,gold) (Node chamber a childList) = if (length list == 0)
                                                                then (Leaf chamber a)
                                                                else (Node chamber a [(findViablePathsHelper (traversePathHelper1 hp x [] (hp,0)) x )|x<-childList,isAlive (hp,gold) x]) 
                                                                where list = [x|x<-childList,isAlive (hp,gold) x] 
                                                                
isAlive (hp,gold) dungeon = if(x>0)
                            then True
                            else False
                            where (x,y) = traversePathHelper1 hp dungeon [] (hp,0)




-- First argument is starting HP
-- Second Argument is dungeon map
-- Find, among the viable paths in the tree (so the nodes you cannot
-- visit is already removed) the two most distant nodes, i.e. the two
-- nodes that are furthest awat from each other.
mostDistantPair :: Integer -> Dungeon -> (Integer, Dungeon)
mostDistantPair hp EmptyTree = (0,EmptyTree)
mostDistantPair hp (Leaf chamber a) = (0,(Leaf chamber a))
mostDistantPair hp (Node chamber a childList) = selectMaxHelper1 0 (findViablePaths hp (Node chamber a childList)) (mostDistantPairHelper (findViablePaths hp (Node chamber a childList)))

sort [] = []
sort ((a,b):xs) = sortHelper (a,b) (sort xs)
                     
sortHelper (a,b) [] = [(a,b)]
sortHelper (a,b) ((m,n):ys) = if (a<m) then ((a,b):(m,n):ys)
                        else ((m,n):(sortHelper (a,b) ys))  

mostDistantPairHelper (Leaf chamber a) = [(0,(Leaf chamber a))]
mostDistantPairHelper (Node chamber a childList) = if (length childList > 1 && findFirst (h1,x1) (h2,x2) [maxHeightandPath x|x<-childList] == 1)
                                                        then [(h1+h2+2,(Node chamber a [x1,x2]))] ++ foldr (++) [] (map mostDistantPairHelper childList)
                                                    else if (length childList > 1 && findFirst (h1,x1) (h2,x2) [maxHeightandPath x|x<-childList] == 2)
                                                        then [(h1+h2+2,(Node chamber a [x2,x1]))] ++ foldr (++) [] (map mostDistantPairHelper childList)
                                                    else
                                                        [(h1+1,(Node chamber a [x1]))] ++ foldr (++) [] (map mostDistantPairHelper childList)
                                                        where 
                                                                (h1,x1) = mostDistantPairHelper2 (Node chamber a childList) (1)
                                                                (h2,x2) = mostDistantPairHelper2 (Node chamber a childList) (2)


findFirst (h1,x1) (h2,x2) ((x,y):xs) = if (x == h1 && x1 == y)
                                        then 1
                                        else if (x == h2 && x2 == y)
                                        then 2
                                        else findFirst (h1,x1) (h2,x2) (xs)
                                            


mostDistantPairHelper2 (Node chamber a childList) i = sort [maxHeightandPath x|x<-childList] !! (len-i)                                    
                                                    where len = length (childList)
                                                    
selectMaxHelper1 max x [] = (max,x)
selectMaxHelper1 max x ((a,b):xs) = if (a>=max)
                                    then selectMaxHelper1 a b (xs)
                                    else selectMaxHelper1 max x (xs)
                                    
selectMaxHelper max x [] = x
selectMaxHelper max x ((a,b):xs) = if (a>=max)
                                    then selectMaxHelper a b (xs)
                                    else selectMaxHelper max x (xs)
                                    
calculateHeight (Leaf chamber a) = 0
calculateHeight (Node chamber a childList) = (maximum [calculateHeight x|x<-childList]) + 1


selectMaxHeight(Node chamber a childList) = selectMaxHelper 0 (Node chamber a childList) [(calculateHeight x, x)|x<-childList]

findPath (Leaf chamber a) = (Leaf chamber a)
findPath (Node chamber a childList) = (Node chamber a ([findPath(selectMaxHeight (Node chamber a childList))]))

maxHeightandPath (Leaf chamber a) = (0,(Leaf chamber a))
maxHeightandPath (Node chamber a childList) = (calculateHeight (Node chamber a childList), findPath (Node chamber a childList))                                  
                                                            

 


-- Find the subtree that has the highest total gold/damage ratio
-- Simply divide the total gold in the subtree by the total damage
-- in the subtree. You only take whole subtrees (i.e you can take a new
-- node as the root of your subtree, but you cannot remove nodes
-- below it). Note that the answer may be the whole tree.
mostEfficientSubtree :: Dungeon -> Dungeon
mostEfficientSubtree EmptyTree = EmptyTree
mostEfficientSubtree (Leaf chamber a) = Leaf chamber a
mostEfficientSubtree (Node chamber a childList) = findMax (mostEfficientSubtreeHelper1 (Node chamber a childList)) y (-x) (Node chamber a childList)
                                                    where 
                                                        (x,y) = mostEfficientSubtreeHelper (Node chamber a childList)

mostEfficientSubtreeHelper1 (Leaf chamber a) = [ratioFinder (Leaf chamber a)]
mostEfficientSubtreeHelper1 (Node chamber a []) = []
mostEfficientSubtreeHelper1 (Node chamber a (x:xs)) = [ratioFinder (Node chamber a (x:xs))] ++ foldr (++) [] (map mostEfficientSubtreeHelper1 (x:xs))

findMax [] maxx1 maxx2 maxDungeon = maxDungeon
findMax (((a,b),(Leaf chamber s)):xs) maxx1 maxx2 maxDungeon = if (a*maxx2 > b*maxx1)
                                                                            then findMax (xs) a b (Leaf chamber s)
                                                                            else findMax (xs) maxx1 maxx2 maxDungeon
findMax (((a,b),(Node chamber s childList)):xs) maxx1 maxx2 maxDungeon = if (a*maxx2 > b*maxx1)
                                                                            then findMax (xs) a b (Node chamber s childList)
                                                                            else findMax (xs) maxx1 maxx2 maxDungeon

ratioFinder (Leaf chamber a) = if (((-1) * x)< 0)
                                then ((y,0),(Leaf chamber a))
                                else ((y,-x),(Leaf chamber a))
                                    where (x,y) = mostEfficientSubtreeHelper (Leaf chamber a)
ratioFinder (Node chamber a childList) = if (((-1) * x) < 0)
                                            then ((y,0),(Node chamber a childList))
                                             else ((y,-x),(Node chamber a childList))
                                                where (x,y) = mostEfficientSubtreeHelper (Node chamber a childList)



mostEfficientSubtreeHelper (Leaf chamber a) = (traversePathHelper 0 (Leaf chamber a) [] (0,0))
mostEfficientSubtreeHelper (Node chamber a childList) = sumTwo (traversePathHelper 0 (Node chamber a childList) [] (0,0)) (sumList (0,0) [mostEfficientSubtreeHelper x|x<-childList])

sumTwo (a,b) (x,y) = (a+x,b+y)

sumList (a,b) [] = (a,b)
sumList (a,b) ((x,y):xs) = sumList (a+x,b+y) xs

