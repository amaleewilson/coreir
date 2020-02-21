module Monitor (input CLK, input handshake_arr_0_ready, input handshake_arr_0_valid, input handshake_arr_1_ready, input handshake_arr_1_valid, input handshake_arr_2_ready, input handshake_arr_2_valid, input handshake_ready, input handshake_valid, input [3:0] in1, input [3:0] in2, input intermediate_tuple__0, input intermediate_tuple__1, input mon_temp1, input mon_temp2, input out);

                        logic temp1, temp2;
                        assign temp1 = |(in1);
                        assign temp2 = &(in1);
                        assert property (@(posedge CLK) handshake_valid -> out === temp1 && temp2);
                    
endmodule

