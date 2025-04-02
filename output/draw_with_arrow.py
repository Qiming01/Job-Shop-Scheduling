import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np
from collections import defaultdict
import sys
from matplotlib.path import Path
import matplotlib.patheffects as PathEffects

# 从文件读取数据
def load_data_from_file(file_path):
    try:
        # 读取CSV文件
        df = pd.read_csv(file_path)

        # 打印列名以进行调试
        print("CSV文件中的列名:", df.columns.tolist())

        # 清理列名（去除前后空格）
        df.columns = df.columns.str.strip()
        print("清理后的列名:", df.columns.tolist())

        # 检查必需的列是否存在
        required_columns = ['id', 'job', 'op', 'machine', 'start', 'end']
        missing_columns = [col for col in required_columns if col not in df.columns]

        if missing_columns:
            print(f"警告: CSV文件缺少以下列: {missing_columns}")
            return None

        # 重命名列以匹配代码其余部分
        column_mapping = {
            'id': 'operation_id',
            'job': 'job_id',
            'op': 'op_number',
            'machine': 'machine_id',
            'start': 'start_time',
            'end': 'end_time'
        }

        # 仅重命名存在的列
        df = df.rename(columns=column_mapping)

        print("重命名后的列名:", df.columns.tolist())
        print(f"成功加载数据。共有 {len(df)} 条调度记录。")
        return df
    except Exception as e:
        print(f"读取文件时出错: {e}")
        import traceback
        traceback.print_exc()
        return None

# 计算关键路径
def find_critical_path(df):
    try:
        # 找到最后完成的时间点
        project_end = df['end_time'].max()

        # 从最后完成的时间点开始，找出关键路径
        critical_path = []
        current_time = project_end

        while current_time > 0:
            # 找到结束时间等于current_time的操作
            candidates = df[df['end_time'] == current_time]
            if len(candidates) == 0:
                break

            # 选择其中一个操作（可能有多个）
            op = candidates.iloc[0]
            critical_path.append(op['operation_id'])
            current_time = op['start_time']  # 更新current_time为该操作的开始时间

            # 如果已经到达项目开始
            if current_time == 0:
                break

        return critical_path
    except Exception as e:
        print(f"计算关键路径时出错: {e}")
        import traceback
        traceback.print_exc()
        return []

# 定义一个函数来绘制带箭头的曲线，连接两个矩形
def draw_arrow(ax, start_x, start_y, end_x, end_y, color='black', linewidth=1.5, arrow_size=0.3, curved=True):
    # 如果操作之间有连接，绘制箭头
    if curved:
        # 贝塞尔曲线的控制点
        middle_x = (start_x + end_x) / 2

        # 调整控制点Y坐标，使曲线更流畅
        control_y = (start_y + end_y) / 2
        if abs(end_y - start_y) < 0.5:  # 如果Y坐标接近，增加曲率
            if start_x < end_x:
                control_y += 0.8
            else:
                control_y -= 0.8

        verts = [
            (start_x, start_y),            # 起点 P0
            (middle_x, control_y),         # 控制点 P1
            (end_x, end_y)                 # 终点 P2
        ]

        codes = [
            Path.MOVETO,
            Path.CURVE3,
            Path.CURVE3
        ]

        path = Path(verts, codes)

        # 绘制路径
        arrow_patch = patches.FancyArrowPatch(
            path=path,
            arrowstyle=f'simple,head_width={arrow_size*2},head_length={arrow_size*2}',
            color=color,
            linewidth=linewidth,
            connectionstyle="arc3,rad=0.2"
        )
    else:
        # 直线箭头
        arrow_patch = patches.FancyArrowPatch(
            (start_x, start_y),
            (end_x, end_y),
            arrowstyle=f'->',
            color=color,
            linewidth=linewidth
        )

    # 添加白色边缘使箭头更加明显
    arrow_patch.set_path_effects([
        PathEffects.Stroke(linewidth=linewidth+1, foreground='white'),
        PathEffects.Normal()
    ])

    ax.add_patch(arrow_patch)

# 绘制甘特图
def plot_gantt_chart(df, critical_ops):
    try:
        # 为不同的作业分配不同的颜色
        unique_jobs = df['job_id'].unique()
        colors = plt.cm.tab10(np.linspace(0, 1, len(unique_jobs)))
        job_colors = {job: colors[i] for i, job in enumerate(unique_jobs)}

        # 获取机器数量
        n_machines = df['machine_id'].nunique()

        # 创建图表
        fig, ax = plt.subplots(figsize=(14, 7))

        # 设置Y轴标签为机器ID
        ax.set_yticks(range(n_machines))
        ax.set_yticklabels([f'Machine {i}' for i in range(n_machines)])

        # 对每个工件的操作按操作编号排序
        job_operations = {}
        for job_id in df['job_id'].unique():
            job_operations[job_id] = df[df['job_id'] == job_id].sort_values('op_number')

        # 记录每个操作的中心位置，用于绘制箭头
        operation_centers = {}  # (job_id, op_number) -> (x, y)

        # 绘制甘特图的每个任务块
        for _, row in df.iterrows():
            op_id = row['operation_id']
            job_id = row['job_id']
            op_num = row['op_number']
            machine_id = row['machine_id']
            start_t = row['start_time']
            end_t = row['end_time']

            # 创建矩形表示每个操作
            rect = patches.Rectangle(
                (start_t, machine_id - 0.4),  # (x, y)
                end_t - start_t,  # width
                0.8,  # height
                linewidth=1,
                edgecolor='black',
                facecolor=job_colors[job_id],
                alpha=0.7
            )
            ax.add_patch(rect)

            # 在矩形中间添加任务ID
            operation_text = f"J{job_id}-{op_num}"
            ax.text(
                start_t + (end_t - start_t) / 2,
                machine_id,
                operation_text,
                ha='center',
                va='center',
                fontsize=9
            )

            # 记录操作中心位置
            center_x = start_t + (end_t - start_t) / 2
            center_y = machine_id
            operation_centers[(job_id, op_num)] = (center_x, center_y)

            # 如果是关键路径上的操作，添加红色边框强调
            if op_id in critical_ops:
                critical_rect = patches.Rectangle(
                    (start_t, machine_id - 0.4),  # (x, y)
                    end_t - start_t,  # width
                    0.8,  # height
                    linewidth=2,
                    edgecolor='red',
                    facecolor='none',
                    linestyle='-',
                    )
                ax.add_patch(critical_rect)

        # 为每个工件的连续操作添加箭头连接
        for job_id, ops in job_operations.items():
            if len(ops) <= 1:
                continue

            # 对于每对连续操作
            for i in range(len(ops) - 1):
                current_op = ops.iloc[i]
                next_op = ops.iloc[i + 1]
                current_op_num = current_op['op_number']
                next_op_num = next_op['op_number']

                # 获取当前操作和下一个操作的中心位置
                if (job_id, current_op_num) in operation_centers and (job_id, next_op_num) in operation_centers:
                    start_x, start_y = operation_centers[(job_id, current_op_num)]
                    end_x, end_y = operation_centers[(job_id, next_op_num)]

                    # 调整箭头起点和终点，使其从矩形边缘开始和结束
                    if start_x < end_x:
                        start_x = current_op['end_time']
                        end_x = next_op['start_time']
                    else:
                        start_x = current_op['start_time']
                        end_x = next_op['end_time']

                    # 绘制从当前操作到下一个操作的箭头
                    draw_arrow(
                        ax,
                        start_x,
                        start_y,
                        end_x,
                        end_y,
                        color=job_colors[job_id],  # 使用工件的颜色
                        linewidth=0.1,
                        arrow_size=0.3,
                        curved=True
                    )


        # 设置X轴和Y轴的范围
        max_time = df['end_time'].max()
        ax.set_xlim(0, max_time * 1.05)
        ax.set_ylim(-0.5, n_machines - 0.5)

        # 添加网格和标题
        ax.grid(True, axis='x', linestyle='--', alpha=0.7)
        ax.set_xlabel('Time')
        ax.set_ylabel('Machines')
        ax.set_title('Job Shop Scheduling Gantt Chart with Operation Flow')

        plt.tight_layout()
        plt.show()
    except Exception as e:
        print(f"绘制甘特图时出错: {e}")
        import traceback
        traceback.print_exc()

# 主程序
def main():
    # 获取命令行参数中的文件路径，或使用默认路径
    if len(sys.argv) > 1:
        file_path = sys.argv[1]
    else:
        file_path = "jssp_schedule.csv"  # 默认文件路径

    print(f"尝试从 {file_path} 加载数据...")

    # 加载数据
    df = load_data_from_file(file_path)

    if df is not None:
        # 计算关键路径
        critical_ops = find_critical_path(df)
        print(f"关键路径操作: {critical_ops}")

        # 绘制甘特图
        plot_gantt_chart(df, critical_ops)
    else:
        print("无法继续，请检查文件路径和格式。")

if __name__ == "__main__":
    main()