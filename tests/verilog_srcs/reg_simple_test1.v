module counter (
    input wire clk,
    input wire rst_n,
    output reg [7:0] count,
    output reg [3:0] comb
);

reg [7:0] count_reg;

always @(*) begin
    if (!rst_n) begin
        comb <= {1'd0, rst_n, 2'd1};
    end
    else begin
        if (count[0]) begin
            comb <= {1'd1, rst_n, 2'd2};
        end
        else begin
            comb <= {1'd0, rst_n, 2'd3};
        end
    end
    count <= count_reg;
end

always @(posedge clk) begin
    if (!rst_n)
        count_reg <= 8'd0;
    else
        count_reg <= count_reg + 8'd1;
end

endmodule
