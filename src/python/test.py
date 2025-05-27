import time
import jsp

# 加载实例
instance = jsp.load_instance("../../instance/ft/ft10.txt")

# 初始化搜索器
tabu = jsp.TabuSearch(instance)

# 设置时间限制
limit_seconds = 20
end_time = time.time() + limit_seconds

# 设置最大迭代次数
max_iteration = 10 ** 8

# 可选最优解
best_makespan = 930


# 停止条件函数：到达时间 or 达到最优值
def stop_condition():
    return time.time() >= end_time or tabu.makespan() == best_makespan


jsp.set_seed(200111)
tabu.search(max_iteration=max_iteration, stop_condition=stop_condition)

# 查看结果
print("Best makespan:", tabu.makespan())
print("Iterations:", tabu.get_iter())
