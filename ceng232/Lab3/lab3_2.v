`timescale 1ns / 1ps
module lab3_2(
			input[5:0] money,
			input CLK,
			input vm, //0:VM0, 1:VM1
			input [2:0] productID, //000:sandwich, 001:chocolate, 11x: dont care
			input sugar, //0: No sugar, 1: With Sugar
			output reg [5:0] moneyLeft,
			output reg [4:0] itemLeft,
			output reg productUnavailable,//1:show warning, 0:do not show warning
			output reg insufficientFund , //1:full, 0:not full
			output reg notExactFund , //1:full, 0:not full
			output reg invalidProduct, //1: empty, 0:not empty
			output reg sugarUnsuitable, //1: empty, 0:not empty
			output reg productReady	//1:door is open, 0:closed
	);

	// Internal State of the Module
	// (you can change this but you probably need this)
	reg [4:0] numOfSandwiches;
	reg [4:0] numOfChocolate;
	reg [4:0] numOfWaterVM1;
	reg [4:0] numOfWaterVM2;
	reg [4:0] numOfCoffee;
	reg [4:0] numOfTea;

	initial
	begin
		numOfSandwiches = 10;
		numOfChocolate = 10;
		numOfWaterVM1 = 5;
		numOfWaterVM2 = 10;
		numOfCoffee = 10;
		numOfTea = 10;
		// You can initialize here
    	// ...
	end

	//Modify the lines below to implement your design
	always @(posedge CLK)
	begin
		moneyLeft = money;
		itemLeft = 0;productUnavailable=0;insufficientFund=0;notExactFund=0;
		invalidProduct=0;sugarUnsuitable=0;productReady=0;
		if (productID == 5 || productID == 6 || productID == 7)
			invalidProduct = 1;
		else if(vm == 0 && (productID == 3 || productID == 4))
			invalidProduct = 1;
		else if(vm == 1 && (productID == 0 || productID == 1))
			invalidProduct = 1;
		else if(productID == 0 && numOfSandwiches == 0)
			productUnavailable = 1;
		else if(productID == 1 && numOfChocolate == 0)
			productUnavailable = 1;
		else if(vm == 0 && productID == 2 && numOfWaterVM1 == 0)
			productUnavailable = 1;
		else if(vm == 1 && productID == 2 && numOfWaterVM2 == 0)
			productUnavailable = 1;
		else if(productID == 3 && numOfCoffee == 0)
			productUnavailable = 1;
		else if(productID == 4 && numOfTea == 0)
			productUnavailable = 1;
		else if(productID == 2 && sugar)
			sugarUnsuitable = 1;
		else if(vm == 0 && productID == 0 && money != 20)
			notExactFund = 1;
		else if(vm == 0 && productID == 1 && money != 10)
			notExactFund = 1;
		else if(vm == 0 && productID == 2 && money != 5)
			notExactFund = 1;
		else if(vm == 1 && productID == 2 && money < 5)
			insufficientFund = 1;
		else if(vm == 1 && productID == 3 && money < 12)
			insufficientFund = 1;
		else if(vm == 1 && productID == 4 && money < 8)
			insufficientFund = 1;
		else if(vm == 0 && productID == 0)
			begin
			numOfSandwiches--;
			itemLeft = numOfSandwiches;
			productReady = 1;
			moneyLeft = 0;
			end
		else if(vm == 0 && productID == 1)
			begin
			numOfChocolate--;
			itemLeft = numOfChocolate;
			productReady = 1;
			moneyLeft = 0;
			end
		else if(vm == 0 && productID == 2)
			begin
			numOfWaterVM1--;
			itemLeft = numOfWaterVM1;
			productReady = 1;
			moneyLeft = 0;
			end
		else if(vm == 1 && productID == 2)
			begin
			numOfWaterVM2--;
			itemLeft = numOfWaterVM2;
			productReady = 1;
			moneyLeft = money - 5;
			end
		else if(vm == 1 && productID == 3)
			begin
			numOfCoffee--;
			itemLeft = numOfCoffee;
			productReady = 1;
			moneyLeft = money - 12;
			end
		else if(vm == 1 && productID == 4)
			begin
			numOfTea--;
			itemLeft = numOfTea;
			productReady = 1;
			moneyLeft = money - 8;
			end
		// You can implement your code here
    	// ...
	end


endmodule



