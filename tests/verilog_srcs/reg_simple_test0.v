module counter (
    input wire clk,
    input wire rst_n,
    output [7:0] count /* 如果这里就算写了reg，也会被识别成wire */
);

reg [7:0] counter_reg;
assign count = counter_reg;

/*
 * 注释测试
 */

always @(posedge clk) begin
    // 测试注释
    if (!rst_n)
        counter_reg <= 8'd0; //;
    else
        counter_reg <= counter_reg + 8'd1;
end

endmodule
