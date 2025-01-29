import random

def cekirdek(sayi: int):
    #Sets the seed for random number generation
    random.seed(sayi)
    
def helper_dogal(boyut, aralik, dagilim):
    if len(boyut) > 1:
        temp = []
        for i in range(boyut[0]):
            temp.append(helper_dogal(boyut[1:], aralik, dagilim))
    else:
        if dagilim == 'uniform':
            temp = []
            for i in range(boyut[0]):
                sayi = random.randint(aralik[0], aralik[1])
                temp.append(sayi)
        else:
            raise ValueError('dagilim variable can be "uniform" or "gaussian" only.')
    return temp
            
def helper_gercek(boyut, aralik, dagilim):
    if len(boyut) > 1:
        temp = []
        for i in range(boyut[0]):
            temp.append(helper_gercek(boyut[1:], aralik, dagilim))
    else:
        if dagilim == 'uniform':
            temp = []
            for i in range(boyut[0]):
                sayi = random.random()
                sayi = aralik[0] + (aralik[1] - aralik[0]) * sayi
                temp.append(sayi)
        else:
            raise ValueError('dagilim variable can be "uniform" or "gaussian" only.')
    return temp
    
def rastgele_dogal(boyut, aralik=(0,100), dagilim='uniform'):
    """
    Generates data of specified dimensions with random integer values and returns a gergen object.

    Parameters:
    boyut (tuple): Shape of the desired data.
    aralik (tuple, optional): (min, max) specifying the range of random values. Defaults to (0,100), which implies a default range.
    dagilim (string, optional): Distribution of random values ('uniform' or other types). Defaults to 'uniform'.

    Returns:
    gergen: A new gergen object with random integer values.
    """
    result = helper_dogal(boyut, aralik, dagilim)
    new_gergen = gergen(result)
    return new_gergen

def rastgele_gercek(boyut, aralik=(0.0, 1.0), dagilim='uniform'):
    """
    Generates a gergen of specified dimensions with random floating-point values.

    Parameters:
    boyut (tuple): Shape of the desired gergen.
    aralik (tuple, optional): (min, max) specifying the range of random values. Defaults to (0.0, 1.0) for uniform distribution.
    dagilim (string, optional): Distribution of random value (e.g., 'uniform', 'gaussian'). Defaults to 'uniform'.

    Returns:
    gergen: A new gergen object with random floating-point values.
    """
    
    result = helper_gercek(boyut, aralik, dagilim)
    new_gergen = gergen(result)
    return new_gergen

class Operation:
    def __call__(self, *operands):
        """
        Makes an instance of the Operation class callable.
        Stores operands and initializes outputs to None.
        Invokes the forward pass of the operation with given operands.

        Parameters:
            *operands: Variable length operand list.

        Returns:
            The result of the forward pass of the operation.
        """
        self.operands = operands
        self.outputs = None
        return self.ileri(*operands)

    def ileri(self, *operands):
        """
        Defines the forward pass of the operation.
        Must be implemented by subclasses to perform the actual operation.

        Parameters:
            *operands: Variable length operand list.

        Raises:
            NotImplementedError: If not overridden in a subclass.
        """
        raise NotImplementedError

import math
from typing import Union

class gergen:

    __veri = None #A nested list of numbers representing the data
    D = None # Transpose of data
    __boyut = None #Dimensions of the derivative (Shape)
    def create_list(self, boyut):
        if len(boyut) > 1:
            temp = []
            for i in range(boyut[-1]):
                temp.append(self.create_list(boyut[:len(boyut)-1]))
        else:
            temp = []
            for i in range(boyut[-1]):
                temp.append(0)
        return temp
    
    def transpose(self, veri, sayi, yol):
        if len(yol) > 1:
            self.transpose(veri[(yol[-1])], sayi, yol[0:len(yol)-1])
        else:
            veri[yol[-1]] = sayi
        
    def visits(self, veri, boyut, yol):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.visits(veri[i], boyut[1:], yol + [i])
        else:
            for i in range(boyut[0]):
                self.transpose(self.D, veri[i], yol + [i])



    def init_helper(self, veri, boyut):
        if len(boyut) > 1:
            temp = []
            for i in range(boyut[0]):
                temp.append(self.init_helper(veri[i], boyut[1:]))
        else:
            temp = []
            for i in range(boyut[0]):
                temp.append(veri[i])
        return temp
            
        
    def __init__(self, veri=None):
    # The constructor for the 'gergen' class.
    #
    # This method initializes a new instance of a gergen object. The gergen can be
    # initialized with data if provided; otherwise, it defaults to None, representing
    # an empty tensor.
    #
    # Parameters:
    # veri (list of lists, optional): A nested list of numbers that represents the
    # gergen data. The outer list contains rows, and each inner list contains the
    # elements of each row. If 'veri' is None, the tensor is initialized without data.
    #
    # Example:
    # To create a tensor with data, pass a nested list:
    # tensor = gergen([[1, 2, 3], [4, 5, 6]])
    #
    # To create an empty tensor, simply instantiate the class without arguments:
    # empty_tensor = gergen()
        if isinstance(veri, int) or isinstance(veri, float):
            self.__veri = veri
        else:
            self.__veri = self.init_helper(veri, tuple(self.helper_boyut(veri, [])))
        self.__boyut = self.boyut()
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            self.D = self.__veri
        else: 
            self.D = self.create_list(self.__boyut)
            self.visits(self.__veri, self.__boyut, [])
        

    def __getitem__(self, index):
    #Indexing for gergen objects
        new_gergen = gergen(self.__veri[index])
        return new_gergen

    def str_helper(self, veri, boyut, flag):
        if len(boyut) > 1:
            temp = "["
            for i in range(boyut[0]-1):
                temp = temp + self.str_helper(veri[i], boyut[1:], False)
            temp = temp + self.str_helper(veri[boyut[0]-1], boyut[1:], True)
        else:
            if(flag):
                temp = str(veri)
            else:
                temp = str(veri) + "\n"
        if len(boyut) > 1 and not flag:
            temp = temp + "]\n"
        elif len(boyut) > 1:
            temp = temp + "]"
        return temp


    def __str__(self):
        #Generates a string representation
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            s1 = "0 boyutlu gergen:\n" + str(self.__veri)
            return s1
        s1 = ""
        for i in range(len(self.__boyut)):
            if i != len(self.__boyut)-1:
                s1 = s1 + str(self.__boyut[i]) + "x"
            else:
                s1 = s1 + str(self.__boyut[i])
        s1 = s1 + " boyutlu gergen:\n"
        s1 = s1 + self.str_helper(self.__veri, self.__boyut, True)
        return s1

    def helper_mul_scalar(self, new_gergen, sayi, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.helper_mul_scalar(new_gergen[i], sayi, boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] = new_gergen[i] * sayi

    def helper_mul_gergen(self, new_gergen, other, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.helper_mul_gergen(new_gergen[i], other[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] *= other[i]
        
    def __mul__(self, other: Union['gergen', int, float]) -> 'gergen':
        """
        Multiplication operation for gergen objects.
        Called when a gergen object is multiplied by another, using the '*' operator.
        Could be element-wise multiplication or scalar multiplication, depending on the context.
        """
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            if isinstance(other.__veri, int) or isinstance(other.__veri, float):
                new_gergen = gergen(self.__veri * other.__veri)
                return new_gergen
            elif isinstance(other.__veri, list):
                return other.__mul__(self)
            else:
                raise TypeError("dimensions of two gergen instances do not align for element-wise multiplication!")
        if isinstance(other, int) or isinstance(other, float):
            new_gergen = gergen(self.__veri)
            self.helper_mul_scalar(new_gergen.__veri, other, self.__boyut)
        elif isinstance(other, gergen):
            if isinstance(other.__veri, int) or isinstance(other.__veri, float):
                return self.__mul__(other.__veri)
            elif self.__boyut == other.__boyut:
                new_gergen = gergen(self.__veri)
                self.helper_mul_gergen(new_gergen.__veri, other.__veri, self.__boyut)
            else:
                raise ValueError('dimensions of two gergen instances do not align for element-wise multiplication!')
        else:
            raise ValueError('incompatible type is provided for other!')
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen
    

    def is_there_zero(self, veri, boyut):
        flag = False
        if len(boyut) > 1:
            for i in range(boyut[0]):
                flag |= self.is_there_zero(veri[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                if veri[i] == 0:
                    return True
        return flag
    def helper_div_scalar(self, new_gergen, other, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.helper_div_scalar(new_gergen[i], other, boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] /= other

    def helper_div_gergen(self, new_gergen, other, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.helper_div_gergen(new_gergen[i], other[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] /= other[i]

    def __truediv__(self, other: Union['gergen', int, float]) -> 'gergen':
        """new_gergen
        Division operation for gergen objects.
        Called when a gergen object is divided by another, using the '/' operator.
        The operation is element-wise.
        """
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            if isinstance(other.__veri, int) or isinstance(other.__veri, float):
                if other.__veri != 0:
                    new_gergen = gergen(self.__veri / other.__veri)
                    return new_gergen
                else:
                    raise ZeroDivisionError('division by zero!')
            else:
                raise TypeError("dimensions of two gergen instances do not align for element-wise division!")
        if isinstance(other, int) or isinstance(other, float):
            if(other != 0):
                new_gergen = gergen(self.__veri)
                self.helper_div_scalar(new_gergen.__veri, other, self.__boyut)
            else:
                raise ZeroDivisionError('division by zero!')
        elif isinstance(other, gergen):
            if isinstance(other.__veri, int) or isinstance(other.__veri, float):
                return self.__truediv__(other.__veri)
            if self.__boyut == other.__boyut:
                if not self.is_there_zero(other.__veri, other.__boyut):
                    new_gergen = gergen(self.__veri)
                    self.helper_div_gergen(new_gergen.__veri, other.__veri, self.__boyut)
                else:
                    raise ZeroDivisionError('division by zero!')
            else:
                raise ValueError('dimensions of two gergen instances do not align for element-wise division!')
        else:
            raise TypeError('incompatible type is provided for other!')
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen


    def helper_add_scalar(self, new_gergen, sayi, boyut):
            if len(boyut) > 1:
                for i in range(boyut[0]):
                    self.helper_add_scalar(new_gergen[i], sayi, boyut[1:])
            else:
                for i in range(boyut[0]):
                    new_gergen[i] = new_gergen[i] + sayi

    def helper_add_gergen(self, new_gergen, other, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.helper_add_gergen(new_gergen[i], other[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] += other[i]

    def __add__(self, other: Union['gergen', int, float]) -> 'gergen':
        """
        Defines the addition operation for gergen objects.
        Called when a gergen object is added to another, using the '+' operator.
        The operation is element-wise.
        """
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            if isinstance(other.__veri, int) or isinstance(other.__veri, float):
                new_gergen = gergen(self.__veri + other.__veri)
                return new_gergen
            elif isinstance(other.__veri, list):
                return other.__add__(self)
            else:
                raise TypeError("dimensions of two gergen instances do not align for element-wise addition!")
        if isinstance(other, int) or isinstance(other, float):
            new_gergen = gergen(self.__veri)
            self.helper_add_scalar(new_gergen.__veri, other, self.__boyut)
        elif isinstance(other, gergen):
            if isinstance(other.__veri, int) or isinstance(other.__veri, float):
                return self.__add__(other.__veri)
            if self.__boyut == other.__boyut:
                new_gergen = gergen(self.__veri)
                self.helper_add_gergen(new_gergen.__veri, other.__veri, self.__boyut)
            else:
                raise ValueError('dimensions of two gergen instances do not align for element-wise addition!')
        else:
            raise ValueError('incompatible type is provided for other!')
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen
    

    def helper_sub_scalar(self, new_gergen, sayi, boyut):
            if len(boyut) > 1:
                for i in range(boyut[0]):
                    self.helper_sub_scalar(new_gergen[i], sayi, boyut[1:])
            else:
                for i in range(boyut[0]):
                    new_gergen[i] = new_gergen[i] - sayi

    def helper_sub_gergen(self, new_gergen, other, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.helper_sub_gergen(new_gergen[i], other[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] -= other[i]


    def __sub__(self, other: Union['gergen', int, float]) -> 'gergen':
        """
        Subtraction operation for gergen objects.
        Called when a gergen object is subtracted from another, using the '-' operator.
        The operation is element-wise.
        """
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            if isinstance(other.__veri, int) or isinstance(other.__veri, float):
                new_gergen = gergen(self.__veri - other.__veri)
                return new_gergen
            else:
                raise TypeError("dimensions of two gergen instances do not align for element-wise subtraction!")
        if isinstance(other, int) or isinstance(other, float):
            new_gergen = gergen(self.__veri)
            self.helper_sub_scalar(new_gergen.__veri, other, self.__boyut)
        elif isinstance(other, gergen):
            if isinstance(other.__veri, int) or isinstance(other.__veri, float):
                return self.__sub__(other.__veri)
            if self.__boyut == other.__boyut:
                new_gergen = gergen(self.__veri)
                self.helper_sub_gergen(new_gergen.__veri, other.__veri, self.__boyut)
            else:
                raise ValueError('dimensions of two gergen instances do not align for element-wise subtraction!')
        else:
            raise ValueError('incompatible type is provided for other!')
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen
    
    def uzunluk_helper(self, boyut):
        if len(boyut) > 1:
            return boyut[0] * self.uzunluk_helper(boyut[1:])
        else:
            return boyut[0]
        
    def uzunluk(self):
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            return 1
        return self.uzunluk_helper(self.__boyut)
    # Returns the total number of elements in the gergen numpy'da yok!!!!!!!!!!!!!!!!!
        

    def helper_boyut(self, veri, boyut):
        if isinstance(veri[0], int) or isinstance(veri[0], float) :
            return boyut + [len(veri)]
        else:
            return self.helper_boyut(veri[0], boyut + [len(veri)])
        
    def boyut(self):
    # Returns the shape of the gergen
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            return ()
        return tuple(self.helper_boyut(self.__veri, []))

    def devrik(self):
    # Returns the transpose of gergen
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_gergen = gergen(self.__veri)
            return new_gergen
        self.visits(self.__veri, self.__boyut, [])
        new_gergen = gergen(self.D)
        return new_gergen

    def sin_helper(self, new_gergen, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.sin_helper(new_gergen[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] = math.sin(new_gergen[i])
            
    def sin(self):
    #Calculates the sine of each element in the given `gergen`.
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_gergen = gergen(math.sin(self.__veri))
            return new_gergen
        new_gergen = gergen(self.__veri)
        self.sin_helper(new_gergen.__veri, new_gergen.__boyut)
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen
    
    def cos_helper(self, new_gergen, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.cos_helper(new_gergen[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] = math.cos(new_gergen[i])

    def cos(self):
    #Calculates the cosine of each element in the given `gergen`.
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_gergen = gergen(math.cos(self.__veri))
            return new_gergen
        new_gergen = gergen(self.__veri)
        self.cos_helper(new_gergen.__veri, new_gergen.__boyut)
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen
    
    def tan_helper(self, new_gergen, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.tan_helper(new_gergen[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] = math.tan(new_gergen[i])

    def tan(self):
    #Calculates the tangent of each element in the given `gergen`.
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_gergen = gergen(math.tan(self.__veri))
            return new_gergen
        new_gergen = gergen(self.__veri)
        self.tan_helper(new_gergen.__veri, new_gergen.__boyut)
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen
    
    def us_helper(self, new_gergen, boyut, sayi):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.us_helper(new_gergen[i], boyut[1:], sayi)
        else:
            for i in range(boyut[0]):
                new_gergen[i] = new_gergen[i] ** sayi

    def us(self, n: int):
    #Raises each element of the gergen object to the power 'n'. This is an element-wise operation.,
        if n < 0:
            raise ValueError("n should be an positive integer.")
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_gergen = gergen(math.pow(self.__veri, n))
            return new_gergen
        new_gergen = gergen(self.__veri)
        self.us_helper(new_gergen.__veri, new_gergen.__boyut, n)
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen
    
    def log_helper(self, new_gergen, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.log_helper(new_gergen[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] = math.log10(new_gergen[i])

    def log(self):
    #Applies the logarithm function to each element of the gergen object, using the base 10.
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_gergen = gergen(math.log10(self.__veri))
            return new_gergen
        new_gergen = gergen(self.__veri)
        self.log_helper(new_gergen.__veri, new_gergen.__boyut)
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen

    def ln_helper(self, new_gergen, boyut):
        if len(boyut) > 1:
            for i in range(boyut[0]):
                self.ln_helper(new_gergen[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                new_gergen[i] = math.log(new_gergen[i])

    def ln(self):
    #Applies the natural logarithm function to each element of the gergen object.
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_gergen = gergen(math.log(self.__veri))
            return new_gergen
        new_gergen = gergen(self.__veri)
        self.ln_helper(new_gergen.__veri, new_gergen.__boyut)
        new_gergen = gergen(new_gergen.__veri)
        return new_gergen
    
    def l1_helper(self, new_gergen, boyut):
        sum = 0.0
        if len(boyut) > 1:
            for i in range(boyut[0]):
                sum += self.l1_helper(new_gergen[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                sum += abs(new_gergen[i])
        return sum 

    def L1(self):
    # Calculates and returns the L1 norm
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            return float(abs(self.__veri))
        return float(self.l1_helper(self.__veri, self.__boyut))

    def l2_helper(self, new_gergen, boyut):
        sum = 0
        if len(boyut) > 1:
            for i in range(boyut[0]):
                sum += self.l2_helper(new_gergen[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                sum += new_gergen[i] * new_gergen[i]
        return sum 


    def L2(self):
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            return self.__veri * self.__veri
        return math.sqrt(self.l2_helper(self.__veri, self.__boyut))
        
    def lp_helper(self, new_gergen, boyut, p):
        sum = 0
        if len(boyut) > 1:
            for i in range(boyut[0]):
                sum += self.lp_helper(new_gergen[i], boyut[1:], p)
        else:
            for i in range(boyut[0]):
                sum += math.pow(abs(new_gergen[i]), p)
        return sum
    
    def Lp(self, p):
    # Calculates and returns the Lp norm, where p should be positive integer
        if p < 0:
           raise ValueError('p variable can not be negative integer')
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            return math.pow(abs(self.__veri), p)
        return math.pow(self.lp_helper(self.__veri, self.__boyut, p), 1/p)
    
                
    def listeye(self):
    #Converts the gergen object into a list or a nested list, depending on its dimensions.
        return self.__veri

    def duzlestir_helper(self, veri, boyut):
        if len(boyut) > 1:
            temp = []
            for i in range(boyut[0]):
                temp = temp + self.duzlestir_helper(veri[i], boyut[1:])
        else:
            temp = []
            for i in range(boyut[0]):
                temp = temp + [veri[i]]
        return temp


    def duzlestir(self):
    #Converts the gergen object's multi-dimensional structure into a 1D structure, effectively 'flattening' the object.
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_gergen = gergen([self.__veri])
            return new_gergen
        new_list = self.duzlestir_helper(self.__veri, self.__boyut)
        new_gergen = gergen(new_list)
        return new_gergen
        
    def boyutlandir_helper(self, yeni_boyut, new_list):
        if len(yeni_boyut) > 1:
            temp = []
            for i in range(yeni_boyut[0]):
                temp.append(self.boyutlandir_helper(yeni_boyut[1:], new_list))
        else:
            temp = []
            for i in range(yeni_boyut[0]):
                temp.append(new_list[0])
                new_list.pop(0)
        return temp


    def boyutlandir(self, yeni_boyut):
    #Reshapes the gergen object to a new shape 'yeni_boyut', which is specified as a tuple.
        if not isinstance(yeni_boyut, tuple):
            raise TypeError("expected format is tuple!")
        if isinstance(self.__veri, int) or isinstance(self.__veri, float):
            new_list = [self.__veri]
            new_gergen = gergen(self.boyutlandir_helper(yeni_boyut, new_list))
            return new_gergen
        if self.uzunluk_helper(self.__boyut) != self.uzunluk_helper(yeni_boyut):
            raise ValueError("new dimensions does not match the total number of elements in the original tensor!")
        new_list = self.duzlestir_helper(self.__veri, self.__boyut)
        new_gergen = gergen(self.boyutlandir_helper(yeni_boyut, new_list))
        return new_gergen

    def ic_carpim_helper(self, veri1, veri2):
        result = 0
        for i in range(len(veri1)):
            result += veri1[i] * veri2[i]
        return result
    
    def get_veri(self):
        return self.__veri

    def ic_carpim(self, other):
    #Calculates the inner (dot) product of this gergen object with another.
        if len(self.__boyut) == 1 and len(other.boyut()) == 1:
            return self.ic_carpim_helper(self.__veri, other.get_veri())
        elif not isinstance(self, gergen) or not isinstance(other, gergen):
            raise TypeError("The operands should be a gergen instance!")
        elif len(self.__boyut) > 2 or len(other.boyut()) > 2:
            raise ValueError("There can not be more than 2 dimensions in matrix multiplication!")
        elif self.__boyut[1] != other.boyut()[0]:
            raise ValueError("Incompatible dimensions in matrix multiplication!")
        else:
            new_list = self.create_list((other.boyut()[1], self.__boyut[0]))
            g1 = other.D
            for i in range(self.__boyut[0]):
                for j in range(other.boyut()[1]):
                    new_list[i][j] = self.ic_carpim_helper(self.__veri[i], g1[j])
            new_gergen = gergen(new_list)
            return new_gergen

    def dis_carpim_helper(self, new_list, veri1, veri2):
        for i in range(len(veri1)):
            for j in range(len(veri2)):
                new_list[i][j] = veri1[i] * veri2[j]
    
    def dis_carpim(self, other):
    #Calculates the outer product of this gergen object with another.
        if not isinstance(other, gergen):
            raise TypeError("Both operands must be gergen instances!")
        if len(self.__boyut) != 1 or len(other.boyut()) != 1:
            raise ValueError("Both operands must be 1-D arrays to compute the outer product")
        new_list = self.create_list((other.boyut()[0], self.__boyut[0]))
        self.dis_carpim_helper(new_list, self.__veri, other.get_veri())
        new_gergen = gergen(new_list)
        return new_gergen
    

    def topla_helper(self, veri, boyut, new_gergen, eksen, count):
        if count != eksen:
            if len(boyut) == 2 and eksen > count:
                for i in range(boyut[0]):
                    new_gergen[i] = self.sum_helper(veri[i], boyut[1:])
            elif len(boyut) > 1:
                for i in range(boyut[0]):
                    self.topla_helper(veri[i], boyut[1:], new_gergen[i], eksen, count+1)
            else:
                for i in range(boyut[0]):
                    new_gergen[i] += veri[i]
        else:
            if len(boyut) > 1:
                for i in range(boyut[0]):
                    self.topla_helper(veri[i], boyut[1:], new_gergen, eksen, count+1)
            else:
                for i in range(boyut[0]):
                    new_gergen += veri[i]

    def sum_helper(self, new_gergen, boyut):
        sum = 0
        if len(boyut) > 1:
            for i in range(boyut[0]):
                sum += self.sum_helper(new_gergen[i], boyut[1:])
        else:
            for i in range(boyut[0]):
                sum += new_gergen[i]
        return sum 
    
    def topla(self, eksen=None):
    #Sums up the elements of the gergen object, optionally along a specified axis 'eksen'.
        if eksen == None:
            if isinstance(self.__veri, int) or isinstance(self.__veri, float):
                return self.__veri
            return self.sum_helper(self.__veri, self.__boyut)
        elif isinstance(eksen, int):
            if eksen >= len(self.__boyut):
                raise ValueError("The specified eksen is out of bounds!")
            else:
                new_boyut = list(self.__boyut)
                del new_boyut[eksen]
                new_boyut.reverse()
                if len(new_boyut) == 0:
                    new_list = [self.sum_helper(self.__veri, self.__boyut)]
                    new_gergen = gergen(new_list)
                    return new_gergen
                else:
                    new_list = self.create_list(new_boyut)
                    self.topla_helper(self.__veri, self.__boyut, new_list, eksen, 0)
                    new_gergen = gergen(new_list)
                    return new_gergen
        else:
            raise TypeError("Eksen variable must be an integer or None!")


    def ortalama(self, eksen=None):
    #Calculates the average of the elements of the gergen object, optionally along a specified axis 'eksen'.
        if eksen == None:
            if isinstance(self.__veri, int) or isinstance(self.__veri, float):
                return self.__veri
            return self.sum_helper(self.__veri, self.__boyut)/self.uzunluk()
        elif isinstance(eksen, int):
            if eksen >= len(self.__boyut):
                raise ValueError("The specified eksen is out of bounds!")
            else:
                new_boyut = list(self.__boyut)
                del new_boyut[eksen]
                new_boyut.reverse()
                if len(new_boyut) == 0:
                    new_list = [self.sum_helper(self.__veri, self.__boyut)/self.__boyut[0]]
                    new_gergen = gergen(new_list)
                    return new_gergen
                else:
                    new_list = self.create_list(new_boyut)
                    self.topla_helper(self.__veri, self.__boyut, new_list, eksen, 0)
                    new_gergen = gergen(new_list)
                    n1 = new_gergen.__truediv__(self.__boyut[eksen])
                    return n1 
        else:
            raise TypeError("Eksen variable must be an integer or None!")
        
import numpy as np              # NumPy, for working with arrays/tensors
import time                     # For measuring time

def example_1():
    #Example 1
    boyut = (64, 64)
    g1 = rastgele_gercek(boyut)
    g2 = rastgele_gercek(boyut)

    start = time.time()
    #TODO
    g3 = g1.ic_carpim(g2)
    end = time.time()

    list1 = g1.listeye()
    list2 = g2.listeye()
    n1 = np.array(list1)
    n2 = np.array(list2)
    start_np = time.time()
    #Apply the same equation for NumPy equivalent
    n3 = np.dot(n1, n2)
    end_np = time.time()

    #TODO:
    if np.allclose(g3.listeye(), n3):
        print("The two results from numpy and gergen classes are the same!")
    else:
        print("The two results from numpy and gergen classes are not the same!")
    print("Time taken for gergen:", end-start)
    print("Time taken for numpy:", end_np-start_np)

def example_2():
    #Example 2
    boyut = (4,16,16,16)
    a = rastgele_gercek(boyut)
    b = rastgele_gercek(boyut)
    c = rastgele_gercek(boyut)
    start = time.time()
    result = (a * b + c * a + b * c).ortalama()
    end = time.time()
    list1 = a.listeye()
    list2 = b.listeye()
    list3 = c.listeye()
    n1 = np.array(list1)
    n2 = np.array(list2)
    n3 = np.array(list3)
    start_np = time.time()
    result1 = np.average((n1 * n2 + n3 * n1 + n2 * n3))
    end_np = time.time()

    #TODO:
    #TODO:
    if np.allclose(result, result1):
        print("The two results from numpy and gergen classes are the same!")
    else:
        print("The two results from numpy and gergen classes are not the same!")
    print("Time taken for gergen:", end-start)
    print("Time taken for numpy:", end_np-start_np)

def example_3():
    #Example 3
    #TODO:
    boyut = (3,64,64)
    a = rastgele_gercek(boyut)
    b = rastgele_gercek(boyut)

    start = time.time()
    result = ((a.sin() + b.cos()).us(2)).ln()/8 
    end = time.time()
    list1 = a.listeye()
    list2 = b.listeye()
    n1 = np.array(list1)
    n2 = np.array(list2)
    start_np = time.time()
    result1 = np.log(np.power((np.sin(n1) + np.cos(n2)),2))/8
    end_np = time.time()

    if np.allclose(result.listeye(), result1):
        print("The two results from numpy and gergen classes are the same!")
    else:
        print("The two results from numpy and gergen classes are not the same!")
    print("Time taken for gergen:", end-start)
    print("Time taken for numpy:", end_np-start_np)

