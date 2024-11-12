module counter (
    input wire clk,
    input wire rst_n,
    output [7:0] count
);

reg [7:0] counter_reg;
reg [7:0] aux_counter_reg;
assign count = counter_reg;

always @(posedge clk) begin
    if (!rst_n) begin
        counter_reg <= 8'd0;
        aux_counter_reg <= 8'd0;
    end
    else begin
        counter_reg <= counter_reg + 8'd1;
        aux_counter_reg <= aux_counter_reg;
        if (counter_reg[0] == 1'd1) begin
            aux_counter_reg <= aux_counter_reg + 1;
        end
    end
end

endmodule
