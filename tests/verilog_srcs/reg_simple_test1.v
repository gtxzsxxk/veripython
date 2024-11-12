module counter (
    input wire clk,
    input wire rst_n,
    output reg [7:0] count,
    output reg [3:0] comb
);

always @(*) begin
    if (!clk) begin
        comb <= {clk, rst_n, 2'd1};
    end
    else begin
        if (count[0]) begin
            comb <= {clk, rst_n, 2'd2};
        end
        else begin
            comb <= {clk, rst_n, 2'd3};
        end
    end
end

always @(posedge clk) begin
    if (!rst_n)
        count <= 8'd0;
    else
        count <= count + 8'd1;
end

endmodule
