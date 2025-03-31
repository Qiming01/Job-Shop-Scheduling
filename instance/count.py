import os


def count_txt_files(directory):
    count = 0
    for entry in os.scandir(directory):
        if entry.is_file() and entry.name.endswith('.txt'):
            count += 1
        elif entry.is_dir():
            count += count_txt_files(entry.path)  # 递归查找子目录
    return count


if __name__ == "__main__":
    current_directory = os.getcwd()
    txt_file_count = count_txt_files(current_directory)
    print("---")
    print("当前目录:", current_directory)
    print(f"算例文件数量: {txt_file_count}")
    print("---")
