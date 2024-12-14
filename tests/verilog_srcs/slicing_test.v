module slicing_test(
    // 没有意义的输入
    input [2:0] a,
    input [2:0] b,
    output [2:0] c
);

    //wire [3:0] d = 4'd3;
    /*
    assign c = 0;
    */
    wire [3:0] d = 4'd3;
    wire [2:0] f = 3'd1; /* wire */
    assign c = a[1:0] + b + d + f[1];

endmodule
