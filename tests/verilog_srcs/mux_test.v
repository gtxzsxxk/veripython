module mux_2to1(
    input [2:0] a,
    input [3:0] b,
    input [1:0] sel,
    output [2:0] out
);
    assign out = sel[0] ? b[3:1] : (sel[1] ? a : 3'd2);
endmodule
