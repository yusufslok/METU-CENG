`timescale 1ns / 1ps

module ROM (
input [2:0] addr,
output reg [7:0] dataOut);
	initial begin
	dataOut = 0;
	end
	always @* begin
	if(addr == 0) dataOut = 0;
	else if(addr == 1)
		dataOut = 85;
	else if(addr == 2)
		dataOut = 170;
	else if(addr == 3)
		dataOut = 51;
	else if(addr == 4)
		dataOut = 204;
	else if(addr == 5)
		dataOut = 15;
	else if(addr == 6)
		dataOut = 240;
	else if(addr == 7)
		dataOut = 255;
	end
	// write your ROM code here



endmodule

module Difference_RAM (
input mode,
input [2:0] addr,
input [7:0] dataIn,
input [7:0] mask,
input CLK,
output reg [7:0] dataOut);
reg [7:0] mem [7:0];
	initial begin
	dataOut = 0;
	mem[0]=0;mem[1]=0;mem[2]=0;mem[3]=0;mem[4]=0;mem[5]=0;mem[6]=0;mem[7]=0;
	end
	always@(posedge CLK) begin
	if(mode == 0) begin
	if(dataIn>=mask)
	mem[addr] = dataIn-mask;
	else
	mem[addr] = mask-dataIn;
	end
	end
	always @* begin
	if(mode == 1)begin
	dataOut = mem[addr];
	end
	end
	// write your XOR_RAM code here


endmodule


module EncodedMemory (
input mode,
input [2:0] index,
input [7:0] number,
input CLK,
output [7:0] result);

	wire [7:0] mask;

	ROM R(index, mask);
	Difference_RAM DR(mode, index, number, mask, CLK, result);

endmodule


