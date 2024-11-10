module counter (
    input wire clk,
    input wire rst_n,
    output [7:0] count
);

reg [7:0] counter_reg;
assign count = counter_reg;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n)
        counter_reg <= 8'd0;
    else
        counter_reg <= counter_reg + 8'd1;
end

endmodule
