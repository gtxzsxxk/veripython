module slicing_test(
    input [1:0] a,
    input [2:0] b,
    output [4:0] c
);

    wire [4:0] d = 4'd5;
    assign c = a + b + d;

endmodule
