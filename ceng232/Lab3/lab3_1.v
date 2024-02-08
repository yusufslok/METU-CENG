`timescale 1ns / 1ps

module bh(input B, input H, input clk, output reg Q);

    initial begin
        Q = 1;
    end

    always@(posedge clk) begin
    if(B == 0 && H == 1)
        Q = 1;
    else if(B == 1 && H == 0)
        Q = 0;
    else if(!B && !H)
        Q = !Q;
    end

	
endmodule

module ic1337(// Inputs
              input A0,
              input A1,
              input A2,
              input clk,
              // Outputs
              output Q0,
              output Q1,
              output Z);

	bh a(((A0 & A1) || (!A0 & !A1) | A2), A0 & !A2, clk, Q0);
    bh b(A0 & !A2,(A0 & !A1 & A2), clk, Q1);
    assign Z = (Q0 == Q1);
endmodule