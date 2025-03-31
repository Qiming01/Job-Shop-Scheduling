import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import networkx as nx

def plot_gantt_chart(csv_file):
    # 读取CSV文件
    df = pd.read_csv(csv_file)

    # 绘制甘特图
    fig, ax = plt.subplots(figsize=(12, 6))
    colors = plt.cm.tab10.colors

    for i, row in df.iterrows():
        ax.barh(y=row["machine_id"], left=row["start_time"], width=row["end_time"] - row["start_time"],
                color=colors[row["machine_id"] % len(colors)], edgecolor='black', alpha=0.7)
        ax.text(row["start_time"] + (row["end_time"] - row["start_time"]) / 2, row["machine_id"],
                str(row["operation_id"]), ha='center', va='center', fontsize=10, color='black')

    # 构建任务有向图
    G = nx.DiGraph()
    for _, row in df.iterrows():
        G.add_edge(row["start_time"], row["end_time"], weight=row["end_time"] - row["start_time"])

    # 计算关键路径
    critical_path = nx.dag_longest_path(G)

    # 标记关键路径
    for i in range(len(critical_path) - 1):
        start = critical_path[i]
        end = critical_path[i + 1]
        row = df[(df["start_time"] == start) & (df["end_time"] == end)]
        if not row.empty:
            machine = row.iloc[0]["machine_id"]
            rect = patches.Rectangle((start, machine - 0.4), end - start, 0.8, linewidth=2,
                                     edgecolor='red', facecolor='none', label="Critical Path" if i == 0 else "")
            ax.add_patch(rect)

    ax.set_xlabel("Time")
    ax.set_ylabel("Machine ID")
    ax.set_title("Gantt Chart with Critical Path Highlighted")
    ax.legend()
    plt.grid(axis="x", linestyle="--", alpha=0.7)
    plt.show()

# 调用函数并传入CSV文件路径
csv_file_path = "schedule.csv"  # 请替换为你的CSV文件路径
plot_gantt_chart(csv_file_path)