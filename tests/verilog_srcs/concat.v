module concat_test(
    input [3:0] a,
    input [3:0] b,
    input [1:0] c,
    input single_bit,
    output [7:0] result1,
    output [5:0] result2_0,
    output [11:6] result2_1,
);
    assign result1 = {a[3:1], b, single_bit};
endmodule
