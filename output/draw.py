import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import Patch
import matplotlib.cm as cm
import numpy as np

# 从文件读取CSV数据
csv_file_path = 'jssp_schedule.csv'
df = pd.read_csv(csv_file_path)

# 获取唯一的作业ID
unique_jobs = sorted(df['Job'].unique())
job_count = len(unique_jobs)

# 使用colormap动态生成足够的颜色
cmap = cm.get_cmap('tab20')  # 使用tab20颜色图可提供20种不同颜色
if job_count <= 20:
    # 如果作业数不超过20，直接使用tab20
    job_colors = {job: cmap(i % 20) for i, job in enumerate(unique_jobs)}
else:
    # 如果作业数超过20，使用hsv colormap生成更多颜色
    cmap = cm.get_cmap('hsv')
    job_colors = {job: cmap(i / job_count) for i, job in enumerate(unique_jobs)}

# 创建图表
fig, ax = plt.subplots(figsize=(15, 8))

# 设置y轴（机器）标签 - 0号机器在最上方，依次向下
machines = sorted(df['Machine'].unique())
# 翻转机器顺序，使0号机器在最上方
machines_reversed = machines[::-1]
machine_positions = {m: i for i, m in enumerate(machines_reversed)}
machine_labels = [f'Machine {m}' for m in machines_reversed]

plt.yticks(range(len(machines)), machine_labels)

# 绘制每个操作的条形
for _, row in df.iterrows():
    # 获取任务信息
    id = row['ID']
    job = row['Job']
    operation = row['Operation']
    machine = row['Machine']
    start = row['StartTime']
    end = row['EndTime']
    is_critical = row['IsCritical']
    duration = end - start

    # 获取机器在图表中的位置
    machine_pos = machine_positions[machine]

    # 绘制条形
    bar = ax.barh(
        machine_pos,
        duration,
        left=start,
        height=0.5,
        color=job_colors[job],
        edgecolor='black',
        alpha=0.8
    )

    # 在条形中添加文本标签（工作ID和操作ID）
    ax.text(
        start + duration/2,
        machine_pos,
        #f'J{job}-{operation}',
        f'{id}',
        ha='center',
        va='center',
        color='black',
        fontweight='bold',
        fontsize=8
    )

    # 为关键块添加红色边框
    if is_critical == 1:
        rect = patches.Rectangle(
            (start, machine_pos-0.25),
            duration,
            0.5,
            linewidth=2,
            edgecolor='red',
            facecolor='none'
        )
        ax.add_patch(rect)
#
# # 为每10个作业创建一个图例列
# legend_entries_per_column = 10
# legend_columns = max(1, job_count // legend_entries_per_column + (1 if job_count % legend_entries_per_column else 0))
#
# legend_elements = [Patch(facecolor=job_colors[job], edgecolor='black', label=f'Job {job}')
#                    for job in unique_jobs]
# legend_elements.append(Patch(facecolor='none', edgecolor='red', linewidth=2, label='Critical'))
#
# ax.legend(handles=legend_elements, loc='upper right', ncol=legend_columns, fontsize='small')

# 设置标题和轴标签
ax.set_title('Job Shop Scheduling Gantt Chart')
ax.set_xlabel('Time')
ax.set_ylabel('Machine')

# 设置 x 轴范围
max_time = df['EndTime'].max()
ax.set_xlim(0, max_time * 1.05)

# 设置 y 轴范围
ax.set_ylim(-0.5, len(machines)-0.5)

# 设置网格
ax.grid(True, axis='x', linestyle='--', alpha=0.7)

# 显示图表
plt.tight_layout()
plt.show()