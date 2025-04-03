import pandas as pd
import plotly.graph_objects as go
from plotly.offline import plot
import matplotlib.pyplot as plt
import colorsys
import random

# 从文件读取CSV数据
csv_file_path = 'jssp_schedule.csv'
df = pd.read_csv(csv_file_path)

# 获取唯一的作业ID和机器ID
unique_jobs = sorted(df['Job'].unique())
job_count = len(unique_jobs)
machines = sorted(df['Machine'].unique())

# 创建一个避开红色的colorscale
def generate_colors_avoiding_red(n):
    colors = []
    # 红色在HSV中的色相值大约在0和0.05之间，或者在0.95到1之间
    red_hue_range = [(0, 0.05), (0.95, 1.0)]

    # 生成随机种子，确保每次运行颜色都一致
    random.seed(42)

    # 已使用的色相值
    used_hues = set()

    for i in range(n):
        # 尝试生成一个不在红色范围且未使用过的色相值
        attempts = 0
        while attempts < 100:  # 防止无限循环
            # 生成在0到1之间的随机色相值
            hue = random.random()

            # 检查是否在红色范围内
            in_red_range = any(lower <= hue <= upper for lower, upper in red_hue_range)

            # 检查是否与已使用的色相值太接近
            too_close = any(abs(hue - used_hue) < 0.1 for used_hue in used_hues)

            if not in_red_range and not too_close:
                used_hues.add(hue)
                # 色相值确定，使用较高的饱和度和亮度
                rgb = colorsys.hsv_to_rgb(hue, 0.8, 0.9)
                # 转换为0-255的RGB值然后格式化
                rgb_255 = tuple(int(255 * c) for c in rgb)
                colors.append(f'rgb{rgb_255}')
                break
            attempts += 1

        # 如果尝试多次后仍找不到适合的颜色，使用默认的蓝色
        if attempts >= 100:
            colors.append('rgb(70, 130, 180)')  # 钢蓝色

    return colors

# 生成每个作业的颜色，避开红色系
job_colors = {job: color for job, color in zip(unique_jobs, generate_colors_avoiding_red(job_count))}

# 准备数据
task_data = []
critical_tasks = []  # 存储关键任务的信息

for i, row in df.iterrows():
    job = row['Job']
    op = row['Operation']
    machine = row['Machine']
    start = row['StartTime']
    end = row['EndTime']
    is_critical = row['IsCritical']

    # 添加任务数据
    task = dict(
        Task=f'Machine {machine}',
        Start=start,
        Finish=end,
        Job=job,  # 保存原始作业ID (数字)
        JobLabel=f'Job {job}',  # 带标签的作业ID
        Operation=op,  # 保存原始操作ID (数字)
        OperationLabel=f'Op {op}',  # 带标签的操作ID
        Resource=f'J{job}-O{op}',
        Color=job_colors[job],
        IsCritical=is_critical,
        Machine=machine,  # 保存原始机器编号
        Id=i  # 保存任务在DataFrame中的ID
    )

    task_data.append(task)

    # 如果是关键任务，记录信息
    if is_critical == 1:
        critical_tasks.append(task)

# 为了让0号机器在最上方，我们需要反转机器顺序
unique_machines = sorted(set(task['Task'] for task in task_data),
                         key=lambda x: int(x.split()[-1]), reverse=True)

# 创建机器到位置的映射
machine_positions = {int(machine.split()[-1]): i for i, machine in enumerate(unique_machines)}

# 创建甘特图
fig = go.Figure()

# 添加每个任务的条形
for task in task_data:
    # 准备悬停文本
    hover_text = (
        f"<b>ID:</b> {task['Id']}<br>"
        f"<b>Job:</b> {task['Job']}<br>"
        f"<b>Operation:</b> {task['Operation']}<br>"
        f"<b>Machine:</b> {task['Machine']}<br>"
        f"<b>Start Time:</b> {task['Start']}<br>"
        f"<b>End Time:</b> {task['Finish']}<br>"
        f"<b>Duration:</b> {task['Finish']-task['Start']}<br>"
        f"<b>Critical:</b> {'Yes' if task['IsCritical']==1 else 'No'}"
    )

    # 添加主任务条形
    fig.add_trace(go.Bar(
        x=[task['Finish'] - task['Start']],
        y=[task['Task']],
        orientation='h',
        base=task['Start'],
        marker_color=task['Color'],
        marker_line_color='black',
        marker_line_width=1,
        name=task['JobLabel'],  # 使用job作为名称
        legendgroup=task['JobLabel'],  # 分组图例
        showlegend=False,  # 我们将单独添加图例
        hoverinfo='text',
        hovertext=hover_text,
        customdata=[task['Id']]  # 保存任务ID用于交互
    ))

# 为每个作业添加一个图例项
for job in unique_jobs:
    fig.add_trace(go.Bar(
        x=[0],  # 不显示的条形
        y=[unique_machines[0]],
        orientation='h',
        marker_color=job_colors[job],
        marker_line_color='black',
        marker_line_width=1,
        name=f'Job {job}',
        legendgroup=f'Job {job}',
        showlegend=True,
        visible='legendonly'  # 只在图例中显示
    ))

# 添加关键任务的图例
fig.add_trace(go.Bar(
    x=[0],  # 不显示的条形
    y=[unique_machines[0]],
    orientation='h',
    marker_color='rgba(0,0,0,0)',
    marker_line_color='red',
    marker_line_width=2,
    name='Critical Task',
    showlegend=True,
    visible='legendonly'  # 只在图例中显示
))

# 使用shapes添加红色边框到关键任务
for task in critical_tasks:
    # 使用机器编号获取其在图表中的实际位置
    machine_number = task['Machine']
    y_position = machine_positions[machine_number]

    fig.add_shape(
        type="rect",
        x0=task['Start'],
        y0=y_position - 0.4,  # 稍微调整位置以适应条形
        x1=task['Finish'],
        y1=y_position + 0.4,
        line=dict(color="red", width=2),
        fillcolor="rgba(0,0,0,0)"
    )

# 为每个任务添加固定大小的标签
annotations = []
for task in task_data:
    # 使用机器编号获取其在图表中的实际位置
    machine_number = task['Machine']
    y_position = machine_positions[machine_number]

    # 为标签文字选择合适的颜色（深色背景用白色文字，浅色背景用黑色文字）
    text_color = "black"  # 默认文字颜色

    annotations.append(dict(
        x=(task['Start'] + task['Finish']) / 2,
        y=y_position,
        text=task['Resource'],
        showarrow=False,
        font=dict(size=12, color=text_color, family="Arial, sans-serif"),
        xanchor='center',
        yanchor='middle',
        # 关键：添加这个属性使文本大小在缩放时保持不变
        xref='x',
        yref='y',
        textangle=0
    ))

# 设置布局以铺满屏幕
fig.update_layout(
    title={
        'text': "Job Shop Scheduling Gantt Chart",
        'y': 0.98,
        'x': 0.5,
        'xanchor': 'center',
        'yanchor': 'top',
        'font': {'size': 24}
    },
    barmode='overlay',
    autosize=True,  # 使图表自动调整大小
    height=800,     # 默认高度，会根据屏幕自动调整
    margin=dict(l=150, r=50, t=100, b=80),
    xaxis=dict(
        title="Time",
        title_font=dict(size=18),
        tickfont=dict(size=14),
        showgrid=True,
        gridwidth=1,
        gridcolor='lightgray',
        zeroline=True,
        zerolinewidth=2,
        zerolinecolor='black'
    ),
    yaxis=dict(
        title="Machine",
        title_font=dict(size=18),
        tickfont=dict(size=14),
        categoryorder='array',
        categoryarray=unique_machines,
    ),
    legend=dict(
        title=dict(text="Legend", font=dict(size=16)),
        font=dict(size=14),
        orientation="h",
        yanchor="bottom",
        y=1.02,
        xanchor="right",
        x=1,
        bgcolor='rgba(255,255,255,0.8)',
        bordercolor='black',
        borderwidth=1
    ),
    plot_bgcolor='white',
    paper_bgcolor='white',
    annotations=annotations,  # 添加固定大小的文本注释
    # 使图表铺满屏幕
    template='plotly_white',
    # 添加悬停模式设置
    hovermode='closest'
)

# 添加配置选项以实现响应式设计和增强交互
config = {
    'responsive': True,  # 使图表响应式
    'displayModeBar': True,  # 显示模式栏
    'modeBarButtonsToRemove': ['select2d', 'lasso2d'],  # 移除一些不需要的按钮
    'displaylogo': False,  # 不显示plotly logo
    'toImageButtonOptions': {
        'format': 'png',  # 设置导出图像格式
        'filename': 'gantt_chart',
        'height': 800,
        'width': 1200,
        'scale': 2  # 导出高分辨率图像
    }
}

# 保存为HTML文件，使用配置选项
plot(fig, filename='gantt_chart.html', config=config, auto_open=True)

print("Gantt chart has been saved as 'gantt_chart.html'")