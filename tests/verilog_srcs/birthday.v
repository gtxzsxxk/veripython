/* 数电实验生日密码锁模块 */
module birthday_locker (
    input clk, // 时钟信号
    input rst_n, // 复位信号
    input [9:0] key, // One-hot 编码的按键输入，对应 123...7890
    output unlocked, // 是否已开锁
    output [63:0] status
);

// 密码：919
reg [3:0] internal_state; // 内部状态，0为开始，1为猜对第一位，etc

// 进入锁开状态后，通过组合逻辑输出结果，一种 Moore 状态机
assign unlocked = internal_state == 3;

assign status = unlocked ? {8'h44, 8'h45, 8'h4b, 8'h43, 8'h4f, 8'h4c, 8'h4e, 8'h55} :
    {16'd0, 8'h44, 8'h45, 8'h4b, 8'h43, 8'h4f, 8'h4c};

always @ (posedge clk) begin
    if (!rst_n) begin
        internal_state <= 0;
    end
    else begin
        if (key > 0) begin
            // 状态0，初始状态
            if (internal_state == 0) begin
                // 第一位输入 9，进入下一状态
                if (key[9] == 1) begin
                    internal_state <= 1;
                end
                else begin
                    // 不满足，留在此状态
                    internal_state <= 0;
                end
            end
            else begin
                if (internal_state == 1) begin
                    // 第二位输入 1，进入下一状态
                    if (key[1] == 1) begin
                        internal_state <= 2;
                    end
                    else begin
                        // 输入9，留在本状态
                        if (key[9] == 1) begin
                            internal_state <= 1;
                        end
                        else begin
                            // 密码已经错误，直接回到初态
                            internal_state <= 0;
                        end
                    end
                end
                else begin
                    if (internal_state == 2) begin
                        // 第三位输入9，进入终态
                        if (key[9] == 1) begin
                            internal_state <= 3;
                        end
                        else begin
                            // 密码错误，回到初态
                            internal_state <= 0;
                        end
                    end
                    else begin
                        // 终态
                        if (internal_state == 3) begin
                            // 输入了9，离开锁开状态，回到了第一位输入为9进入的状态
                            if (key[9] == 1) begin
                                internal_state <= 1;
                            end
                            else begin
                                // 用户的输入不对，回到初态
                                internal_state <= 0;
                            end
                        end
                        else begin
                            // 非法状态，进入死锁
                            internal_state <= internal_state;
                        end
                    end
                end
            end
        end
        else begin
            // 用户没有输入，保留在原有状态
            internal_state <= internal_state;
        end
    end
end

endmodule
