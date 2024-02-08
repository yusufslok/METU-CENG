`timescale 1ns / 1ps
module SelectionOfAvatar(
	input [1:0] mode,
	input [5:0] userID,
	input [1:0] candidate, // 00:Air 01:Fire, 10:Earth, 11: Water
	input CLK,
	output reg [1:0] ballotBoxId,
	output reg [5:0] numberOfRegisteredVoters,
	output reg [5:0] numberOfVotesWinner, // number of votes of winner
	output reg [1:0] WinnerId,
	output reg AlreadyRegistered,
	output reg AlreadyVoted,
	output reg NotRegistered,
	output reg VotingHasNotStarted,
	output reg RegistrationHasEnded
 	);
	reg [1:0] mem [3:0][15:0];
	integer a;
	integer j;
	integer i;
	reg [7:0] cs [3:0];
	
	initial begin
		// ...
		numberOfRegisteredVoters=0;
		AlreadyRegistered=0;
		AlreadyVoted=0;
		NotRegistered=0;
		VotingHasNotStarted=0;
		RegistrationHasEnded=0;
		WinnerId=0;
		numberOfVotesWinner=0;
		i=1;
		cs[0] = 0;cs[1] = 0;cs[2] = 0;cs[3] = 0;
		for(j=0;j<4;j++)begin
			for(a=0;a<16;a++)begin
				mem[j][a] = 0;
			end
		end
	end

	always @(posedge CLK)
	begin
		// ...
		AlreadyRegistered=0;
		AlreadyVoted=0;
		NotRegistered=0;
		VotingHasNotStarted=0;
		RegistrationHasEnded=0;
		if(userID <= 15) ballotBoxId = 0;		
			else if(userID <= 31) ballotBoxId = 1;
			else if(userID <= 47) ballotBoxId = 2;
			else if(userID <= 63) ballotBoxId = 3;
		if(i<=100 && mode == 0)begin
			if(mem[ballotBoxId][userID-ballotBoxId*16] == 0) begin 
				mem[ballotBoxId][userID-ballotBoxId*16] = 1;
				numberOfRegisteredVoters++;
			end
			else AlreadyRegistered = 1;
		end
		else if(i<=100 && mode == 1) begin
			VotingHasNotStarted = 1;
		end
		else if(i<=200 && mode == 1)begin
			if(mem[ballotBoxId][userID-ballotBoxId*16] == 1) begin 
				cs[candidate]++;
				mem[ballotBoxId][userID-ballotBoxId*16]++;
			end
			else if(mem[ballotBoxId][userID-ballotBoxId*16] == 2) AlreadyVoted = 1;
			else if(mem[ballotBoxId][userID-ballotBoxId*16] == 0)  NotRegistered = 1;
		end
		else if(i<=200 && mode == 0) RegistrationHasEnded = 1;
		else if(i>200)begin
			for(a=0;a<4;a++)begin
				if(cs[a] > numberOfVotesWinner)begin
					numberOfVotesWinner = cs[a];
					WinnerId = a;
				end
			end
		end

		i++;
	end
endmodule
