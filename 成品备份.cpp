#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits> // Required for numeric_limits
#include <chrono>
#include <thread>
#include <map>

// Forward declarations
class Expense;
class Date;
class ExpenseManager;
class UIManager;

// --- Helper Enums and Structs ---

enum class Category {
	LEARNING_SUPPLIES,
	DAILY_NECESSITIES,
	TRANSPORTATION,
	FOOD,
	OTHER,
	INVALID // For error handling
};

std::string categoryToString(Category category) {
	switch (category) {
		case Category::LEARNING_SUPPLIES: return "学习用品";
		case Category::DAILY_NECESSITIES: return "日用品";
		case Category::TRANSPORTATION:    return "交通费";
		case Category::FOOD:              return "伙食费";
		case Category::OTHER:             return "其他";
		default:                          return "未知分类";
	}
}

Category stringToCategory(const std::string& s) {
	if (s == "学习用品") return Category::LEARNING_SUPPLIES;
	if (s == "日用品") return Category::DAILY_NECESSITIES;
	if (s == "交通费") return Category::TRANSPORTATION;
	if (s == "伙食费") return Category::FOOD;
	if (s == "其他") return Category::OTHER;
	// For direct enum int value loading from file
	if (s == "0") return Category::LEARNING_SUPPLIES;
	if (s == "1") return Category::DAILY_NECESSITIES;
	if (s == "2") return Category::TRANSPORTATION;
	if (s == "3") return Category::FOOD;
	if (s == "4") return Category::OTHER;
	return Category::INVALID;
}

// --- Date Class ---
class Date {
public:
	int year, month, day;
	
	Date(int y = 1970, int m = 1, int d = 1) : year(y), month(m), day(d) {}
	
	bool isValid() const {
		if (year < 1900 || year > 2100) return false;
		if (month < 1 || month > 12) return false;
		if (day < 1 || day > 31) return false; // Simplified validation
		// Further validation for days in month can be added
		if (month == 2) {
			bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
			if (day > (isLeap ? 29 : 28)) return false;
		} else if (month == 4 || month == 6 || month == 9 || month == 11) {
			if (day > 30) return false;
		}
		return true;
	}
	
	// For sorting
	bool operator<(const Date& other) const {
		if (year != other.year) return year < other.year;
		if (month != other.month) return month < other.month;
		return day < other.day;
	}
	
	// For display and storage
	friend std::ostream& operator<<(std::ostream& os, const Date& date) {
		os << std::setfill('0') << std::setw(4) << date.year << "-"
		<< std::setfill('0') << std::setw(2) << date.month << "-"
		<< std::setfill('0') << std::setw(2) << date.day;
		return os;
	}
};

// --- Expense Class ---
class Expense {
private:
	int id;
	double amount;
	Category category;
	Date date;
	std::string remarks;
	
public:
	Expense(int id, double amount, Category category, const Date& date, const std::string& remarks)
	: id(id), amount(amount), category(category), date(date), remarks(remarks) {}
	
	// Getters
	int getId() const { return id; }
	double getAmount() const { return amount; }
	Category getCategory() const { return category; }
	const Date& getDate() const { return date; }
	const std::string& getRemarks() const { return remarks; }
	
	// For display
	void display() const {
		std::cout << std::left << std::setw(5) << id
		<< std::setw(12) << std::fixed << std::setprecision(2) << amount
		<< std::setw(18) << categoryToString(category)
		<< std::setw(15) << date
		<< remarks << std::endl;
	}
};

// --- Storage Manager Module (Conceptually) ---
// For simplicity, its methods are part of ExpenseManager in this single-file version.
// In a multi-file project, this would be a separate class.
class StorageManager {
public:
	static bool saveExpenses(const std::string& filename, const std::vector<Expense>& expenses, int nextId) {
		std::ofstream outFile(filename);
		if (!outFile) {
			std::cerr << "错误: 无法打开文件 " << filename << " 进行写入!" << std::endl;
			return false;
		}
		
		outFile << nextId << std::endl; // Save nextId first
		
		for (const auto& expense : expenses) {
			outFile << expense.getId() << std::endl;
			outFile << expense.getAmount() << std::endl;
			outFile << static_cast<int>(expense.getCategory()) << std::endl; // Store category as int
			outFile << expense.getDate().year << std::endl;
			outFile << expense.getDate().month << std::endl;
			outFile << expense.getDate().day << std::endl;
			outFile << expense.getRemarks() << std::endl; // Remarks last, can contain spaces
		}
		outFile.close();
		return true;
	}
	
	static bool loadExpenses(const std::string& filename, std::vector<Expense>& expenses, int& nextId) {
		std::ifstream inFile(filename);
		if (!inFile) {
			// This is not an error if the file doesn't exist on first run
			// std::cerr << "信息: 文件 " << filename << " 未找到, 将创建新文件." << std::endl;
			nextId = 1; // Initialize nextId if file doesn't exist
			return false;
		}
		
		expenses.clear();
		std::string line;
		
		// Load nextId
		if (!std::getline(inFile, line)) {
			nextId = 1; return false; // Empty or corrupt file
		}
		try {
			nextId = std::stoi(line);
		} catch (const std::invalid_argument& ia) {
			std::cerr << "错误: 文件中 nextId 格式无效。" << std::endl;
			nextId = 1; return false;
		}
		
		
		int id;
		double amount;
		int categoryInt;
		int year, month, day;
		std::string remarks;
		
		while (std::getline(inFile, line)) { // Read ID
			try {
				id = std::stoi(line);
				if (!std::getline(inFile, line)) break; // Amount
				amount = std::stod(line);
				if (!std::getline(inFile, line)) break; // Category
				categoryInt = std::stoi(line);
				if (!std::getline(inFile, line)) break; // Year
				year = std::stoi(line);
				if (!std::getline(inFile, line)) break; // Month
				month = std::stoi(line);
				if (!std::getline(inFile, line)) break; // Day
				day = std::stoi(line);
				if (!std::getline(inFile, line)) break; // Remarks
				remarks = line;
				
				Date date(year, month, day);
				Category cat = static_cast<Category>(categoryInt);
				
				if (!date.isValid() || cat == Category::INVALID) {
					std::cerr << "警告: 跳过无效的记录 (ID: " << id << ")" << std::endl;
					continue;
				}
				expenses.emplace_back(id, amount, cat, date, remarks);
			} catch (const std::exception& e) {
				std::cerr << "错误: 解析文件时发生错误: " << e.what() << std::endl;
				// Attempt to recover or signal critical failure
				return false; // Or try to skip problematic entry
			}
		}
		inFile.close();
		return true;
	}
};


// --- Expense Manager Class ---
class ExpenseManager {
private:
	std::vector<Expense> expenses;
	int nextId; // To assign unique IDs to expenses
	std::string dataFile;
	
public:
	ExpenseManager(const std::string& filename = "expenses.txt") : nextId(1), dataFile(filename) {
		loadData();
	}
	
	~ExpenseManager() {
		saveData();
	}
	
	void addExpense(double amount, Category category, const Date& date, const std::string& remarks) {
		expenses.emplace_back(nextId++, amount, category, date, remarks);
		std::cout << "开销已添加 (ID: " << expenses.back().getId() << ")." << std::endl;
	}
	
	bool deleteExpense(int id) {
		auto it = std::find_if(expenses.begin(), expenses.end(),
			[id](const Expense& e) { return e.getId() == id; });
		
		if (it != expenses.end()) {
			std::cout << "找到开销记录:" << std::endl;
			it->display();
			std::cout << "\n准备删除此记录。确认删除将在15秒后进行。" << std::endl;
			std::cout << "请在倒计时结束前确认 (输入 'y' 确认, 其他任意键取消):" << std::endl;
			
			bool confirmed = false;
			bool cancelled = false;
			std::thread timer_thread([&]() {
				for (int i = 15; i > 0; --i) {
					if (cancelled) return; // Check if main thread cancelled
					std::cout << "\r倒计时: " << i << " 秒... " << std::flush;
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			});
			
			char confirm_char = ' ';
			// This simple input might not interrupt sleep properly on all systems/compilers
			// A more robust solution needs platform-specific non-blocking input
			std::cin >> confirm_char;
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
			
			cancelled = true; // Signal timer thread to stop (if it hasn't finished)
			if (timer_thread.joinable()) {
				timer_thread.join(); // Wait for timer thread to finish
			}
			std::cout << "\r倒计时结束。            " << std::endl;
			
			
			if (confirm_char == 'y' || confirm_char == 'Y') {
				confirmed = true;
			}
			
			
			if (confirmed) {
				expenses.erase(it);
				std::cout << "开销记录 (ID: " << id << ") 已删除。" << std::endl;
				return true;
			} else {
				std::cout << "删除操作已取消。" << std::endl;
				return false;
			}
		} else {
			std::cout << "未找到ID为 " << id << " 的开销记录。" << std::endl;
			return false;
		}
	}
	
	void displayAllExpenses(const std::string& title = "所有开销记录") const {
		std::cout << "\n--- " << title << " ---" << std::endl;
		if (expenses.empty()) {
			std::cout << "没有开销记录。" << std::endl;
			return;
		}
		std::cout << std::left << std::setw(5) << "ID"
		<< std::setw(12) << "金额(元)"
		<< std::setw(18) << "分类"
		<< std::setw(15) << "日期"
		<< "备注" << std::endl;
		std::cout << std::string(70, '-') << std::endl;
		for (const auto& expense : expenses) {
			expense.display();
		}
		std::cout << std::string(70, '-') << std::endl;
	}
	
	void sortExpensesByDate() {
		std::sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
			return a.getDate() < b.getDate();
		});
		std::cout << "开销已按日期排列。" << std::endl;
	}
	
	void sortExpensesByAmount() {
		std::sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
			return a.getAmount() < b.getAmount(); // Ascending by default, use > for descending
		});
		std::cout << "开销已按金额排列 (从小到大)。" << std::endl;
	}
	
	void showStatistics() const {
		if (expenses.empty()) {
			std::cout << "没有开销记录可供统计。" << std::endl;
			return;
		}
		
		std::cout << "\n--- 开销统计与分析 ---" << std::endl;
		
		double totalExpenses = 0;
		std::map<Category, double> expensesByCategory;
		std::map<Category, int> countByCategory;
		
		for (const auto& expense : expenses) {
			totalExpenses += expense.getAmount();
			expensesByCategory[expense.getCategory()] += expense.getAmount();
			countByCategory[expense.getCategory()]++;
		}
		
		std::cout << "总开销: " << std::fixed << std::setprecision(2) << totalExpenses << " 元" << std::endl;
		std::cout << "总记录数: " << expenses.size() << " 条" << std::endl;
		if (expenses.size() > 0) {
			std::cout << "平均每条开销: " << std::fixed << std::setprecision(2) << totalExpenses / expenses.size() << " 元" << std::endl;
		}
		
		
		std::cout << "\n按分类统计:" << std::endl;
		Category maxSpendingCategory = Category::INVALID;
		double maxSpend = 0;
		
		for (const auto& pair : expensesByCategory) {
			std::cout << "  - " << std::setw(15) << std::left << categoryToString(pair.first) + ":"
			<< std::fixed << std::setprecision(2) << pair.second << " 元"
			<< " (" << countByCategory[pair.first] << " 条)" << std::endl;
			if (pair.second > maxSpend) {
				maxSpend = pair.second;
				maxSpendingCategory = pair.first;
			}
		}
		
		if (maxSpendingCategory != Category::INVALID) {
			std::cout << "\n花费最多的分类: " << categoryToString(maxSpendingCategory) 
			<< " (共 " << std::fixed << std::setprecision(2) << maxSpend << " 元)" << std::endl;
		}
		
		// Simple analysis: e.g. highest single expense
		if (!expenses.empty()) {
			auto maxElementIt = std::max_element(expenses.begin(), expenses.end(), 
				[](const Expense& a, const Expense& b){
					return a.getAmount() < b.getAmount();
				});
			std::cout << "单笔最高开销: " << std::fixed << std::setprecision(2) << maxElementIt->getAmount() << " 元 (分类: "
			<< categoryToString(maxElementIt->getCategory()) << ", 日期: " << maxElementIt->getDate() << ")"
			<< std::endl;
		}
		std::cout << "------------------------" << std::endl;
	}
	
	
private:
	void loadData() {
		if (!StorageManager::loadExpenses(dataFile, expenses, nextId)) {
			std::cout << "信息: 未找到数据文件或文件为空，将开始新的记录。" << std::endl;
			// Ensure nextId is at least 1 and greater than any loaded ID
			if (expenses.empty()) {
				nextId = 1;
			} else {
				int maxLoadedId = 0;
				for(const auto& exp : expenses) {
					if (exp.getId() > maxLoadedId) {
						maxLoadedId = exp.getId();
					}
				}
				nextId = maxLoadedId + 1;
			}
		} else {
			std::cout << "数据已从 " << dataFile << " 加载。" << std::endl;
		}
	}
	
	void saveData() const {
		if (StorageManager::saveExpenses(dataFile, expenses, nextId)) {
			std::cout << "数据已保存到 " << dataFile << "." << std::endl;
		} else {
			std::cerr << "错误: 保存数据失败!" << std::endl;
		}
	}
};


// --- UI Manager Class (Handles CMD Interface) ---
class UIManager {
private:
	ExpenseManager& manager; // Reference to the manager
	
	// Helper to clear input buffer
	void clearInputBuffer() {
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	
	// Helper to get validated double input
	double getValidatedDouble(const std::string& prompt) {
		double value;
		while (true) {
			std::cout << prompt;
			std::cin >> value;
			if (std::cin.good() && value >= 0) { // Amount should be non-negative
				clearInputBuffer();
				return value;
			} else {
				std::cout << "输入无效，请输入一个非负数字。" << std::endl;
				std::cin.clear();
				clearInputBuffer();
			}
		}
	}
	
	// Helper to get validated int input
	int getValidatedInt(const std::string& prompt, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max()) {
		int value;
		while (true) {
			std::cout << prompt;
			std::cin >> value;
			if (std::cin.good() && value >= minVal && value <= maxVal) {
				clearInputBuffer();
				return value;
			} else {
				std::cout << "输入无效。请输入一个在 " << minVal << " 和 " << maxVal << " 之间的整数。" << std::endl;
				std::cin.clear();
				clearInputBuffer();
			}
		}
	}
	
	
public:
	UIManager(ExpenseManager& mgr) : manager(mgr) {}
	
	void displayMenu() {
		std::cout << "\n===== 大学生开销记录系统 =====" << std::endl;
		std::cout << "1. 添加开销记录" << std::endl;
		std::cout << "2. 删除开销记录" << std::endl;
		std::cout << "3. 显示所有开销记录" << std::endl;
		std::cout << "4. 按日期排列记录" << std::endl;
		std::cout << "5. 按金额排列记录" << std::endl;
		std::cout << "6. 数据统计与分析" << std::endl;
		std::cout << "0. 退出程序" << std::endl;
		std::cout << "============================" << std::endl;
		std::cout << "请输入您的选择: ";
	}
	
	void processUserInput() {
		int choice;
		while (true) {
			displayMenu();
			std::cin >> choice;
			
			if (std::cin.fail()) {
				std::cout << "无效输入，请输入数字。" << std::endl;
				std::cin.clear();
				clearInputBuffer();
				continue;
			}
			clearInputBuffer(); // Clear the rest of the line including newline
			
			switch (choice) {
				case 1: handleAddExpense(); break;
				case 2: handleDeleteExpense(); break;
				case 3: manager.displayAllExpenses(); break;
				case 4: {
					manager.sortExpensesByDate();
					manager.displayAllExpenses("按日期排列后的开销");
					break;
				}
				case 5: {
					manager.sortExpensesByAmount();
					manager.displayAllExpenses("按金额排列后的开销");
					break;
				}
				case 6: manager.showStatistics(); break;
			case 0:
				std::cout << "感谢使用，正在退出..." << std::endl;
				return; // Exit loop and program
			default:
				std::cout << "无效选择，请重试。" << std::endl;
			}
			std::cout << "\n按 Enter键 继续...";
			std::cin.get(); // Wait for user to press Enter
		}
	}
	
private:
	void handleAddExpense() {
		std::cout << "\n--- 添加新的开销记录 ---" << std::endl;
		double amount = getValidatedDouble("请输入金额: ");
		
		std::cout << "请选择分类:" << std::endl;
		std::cout << "1. " << categoryToString(Category::LEARNING_SUPPLIES) << std::endl;
		std::cout << "2. " << categoryToString(Category::DAILY_NECESSITIES) << std::endl;
		std::cout << "3. " << categoryToString(Category::TRANSPORTATION) << std::endl;
		std::cout << "4. " << categoryToString(Category::FOOD) << std::endl;
		std::cout << "5. " << categoryToString(Category::OTHER) << std::endl;
		int categoryChoice = getValidatedInt("输入分类编号 (1-5): ", 1, 5);
		
		Category category = static_cast<Category>(categoryChoice - 1); // Enum starts from 0
		
		Date date;
		while(true) {
			int year = getValidatedInt("请输入年份 (YYYY): ", 1900, 2100);
			int month = getValidatedInt("请输入月份 (MM): ", 1, 12);
			int day = getValidatedInt("请输入日期 (DD): ", 1, 31);
			date = Date(year, month, day);
			if (date.isValid()) break;
			std::cout << "日期无效，请重新输入。" << std::endl;
		}
		
		
		std::string remarks;
		std::cout << "请输入备注 (可选): ";
		std::getline(std::cin, remarks); // std::cin was already cleared by getValidatedInt
		
		manager.addExpense(amount, category, date, remarks);
	}
	
	void handleDeleteExpense() {
		std::cout << "\n--- 删除开销记录 ---" << std::endl;
		manager.displayAllExpenses("当前开销记录 (供参考)");
		if (getValidatedInt("是否有记录需要删除？(1代表是, 0代表否): ", 0, 1) == 0) return;
		
		int id = getValidatedInt("请输入要删除的开销记录ID: ");
		manager.deleteExpense(id);
	}
};


// --- Main Application Logic ---
int main() {
	ExpenseManager expenseManager("student_expenses.dat"); // Specify data file name
	UIManager ui(expenseManager);
	
	ui.processUserInput(); // Start the main loop
	
	return 0;
}


