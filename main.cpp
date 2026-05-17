#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <limits>
using namespace std;

// ==============================
// 資料結構
// ==============================
struct Point {
    int x;
    int y;
};

// ==============================
// 工具函式
// ==============================
void printPoints(const vector<Point>& pts, const string& title) {
    cout << title << "\n";
    for (const auto& p : pts) {
        cout << "(" << p.x << "," << p.y << ") ";
    }
    cout << "\n\n";
}

double distancePoints(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

bool cmpX(const Point& a, const Point& b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
}

bool cmpY(const Point& a, const Point& b) {
    if (a.y != b.y) return a.y < b.y;
    return a.x < b.x;
}

// =====================================================
// Part A. 2D_Maximal Template
// =====================================================

// 判斷 p1 是否被 p2 凌駕
// 若 p2.x >= p1.x 且 p2.y >= p1.y，並且至少一項嚴格大於，則 p1 被 p2 凌駕
bool isDominated(const Point& p1, const Point& p2) {
    return (p2.x >= p1.x && p2.y >= p1.y) && (p2.x > p1.x || p2.y > p1.y);
}

// n <= 3 時直接暴力求極點
vector<Point> bruteForceMaximal(const vector<Point>& S) {
    vector<Point> result;

    // 1. 對每個點檢查是否被其他點凌駕
    for (int i = 0; i < S.size(); i++) {
        bool dominated = false;
        for (int j = 0; j < S.size(); j++) {
            if (i != j && isDominated(S[i], S[j])) {
                dominated = true;
                break;
            }
        }
        // 2. 若沒有被任何點凌駕，則加入 result
        if (!dominated) {
            result.push_back(S[i]);
        }
    }
    // 3. 回傳 result
    return result;
}

// 取得 x 座標中位數
int getMedianX(const vector<Point>& S) {
    vector<int> xs;
    for (const auto& p : S) {
        xs.push_back(p.x);
    }

    // 1. 將 xs 排序
    sort(xs.begin(), xs.end());
    
    // 2. 取中位數並回傳
    return xs[xs.size() / 2]; 
}

// 2D_Maximal 主遞迴
vector<Point> maximalPoints(const vector<Point>& S) {
    int n = S.size();

    if (n <= 3) {
        return bruteForceMaximal(S);
    }

    // Step 1: 找 x 座標中位數
    int medianX = getMedianX(S);

    // Step 2: 分割成 S_L 與 S_R
    vector<Point> SL, SR;

    // 將 x <= medianX 的點放入 SL
    // 將 x >  medianX 的點放入 SR
    for (const auto& p : S) {
        if (p.x <= medianX) {
            SL.push_back(p);
        } else {
            SR.push_back(p);
        }
    }

    // 避免分割失敗造成無限遞迴
    if (SL.size() == S.size() || SR.size() == S.size()) {
        return bruteForceMaximal(S);
    }

    // Step 3: 遞迴求左右兩側極點
    vector<Point> leftMaximal = maximalPoints(SL);
    vector<Point> rightMaximal = maximalPoints(SR);

    // Step 4: 找出 S_R 極點中 y 最大值
    int ymax = -1;
    // 從 rightMaximal 中找出最大的 y 值
    for (const auto& p : rightMaximal) {
        if (p.y > ymax) {
            ymax = p.y;
        }
    }

    // Step 5: 刪除 S_L 中 y < ymax 的點
    vector<Point> filteredLeft;

    // 若 rightMaximal 為空，leftMaximal 全保留
    // 否則只保留 y >= ymax 的左側極點
    if (rightMaximal.empty()) {
        filteredLeft = leftMaximal;
    } else {
        for (const auto& p : leftMaximal) {
            if (p.y >= ymax) {
                filteredLeft.push_back(p);
            }
        }
    }

    // Step 6: 合併結果
    vector<Point> result;

    // 將 filteredLeft 與 rightMaximal 合併到 result
    result = rightMaximal;
    for (const auto& p : filteredLeft) {
        result.push_back(p);
    }

    return result;
}

// =====================================================
// Part B. 2D_Closest_Pair Template
// =====================================================

// n <= 3 時直接暴力求最近距離
double bruteForceClosest(const vector<Point>& S) {
    int n = S.size();
    // 1. 若點數小於 2，可回傳很大的值
    if (n < 2) return numeric_limits<double>::infinity();
    
    double minDist = numeric_limits<double>::infinity();
    // 2. 兩兩比較所有點距離
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            minDist = min(minDist, distancePoints(S[i], S[j]));
        }
    }
    // 3. 回傳最小距離
    return minDist; 
}

// 2D Closest Pair 遞迴函式
double closestPairRecursive(vector<Point> Px, vector<Point> Py) {
    int n = Px.size();

    // Base case
    if (n <= 3) {
        return bruteForceClosest(Px);
    }

    // Step 1: 取 x 中位數
    int mid = n / 2;
    Point midPoint = Px[mid];
    int L = midPoint.x;

    // Step 2: 分割成左半與右半
    vector<Point> PxL(Px.begin(), Px.begin() + mid);
    vector<Point> PxR(Px.begin() + mid, Px.end());

    vector<Point> PyL, PyR;

    // 計算 PxL 中有多少個 x == L 的點，確保 PyL 裝入剛好數量的中線點
    int leftLCount = 0;
    for (const auto& p : PxL) {
        if (p.x == L) leftLCount++;
    }

    // 依照題目中的規則，把 Py 中各點分到 PyL, PyR，同時維持 y 的排序
    for (const auto& p : Py) {
        if (p.x < L) {
            PyL.push_back(p);
        } else if (p.x > L) {
            PyR.push_back(p);
        } else {
            if (leftLCount > 0) {
                PyL.push_back(p);
                leftLCount--;
            } else {
                PyR.push_back(p);
            }
        }
    }

    // 避免分割失敗造成無限遞迴
    if (PxL.empty() || PxR.empty()) {
        return bruteForceClosest(Px);
    }

    // Step 3: 遞迴求左右最近距離
    double dL = closestPairRecursive(PxL, PyL);
    double dR = closestPairRecursive(PxR, PyR);

    double delta = min(dL, dR);

    // Step 4: 建立 strip
    vector<Point> strip;

    // 將所有滿足 |x - L| < delta 的點放入 strip
    // 建議從 Py 取，因為 Py 已按 y 排序
    for (const auto& p : Py) {
        if (abs(p.x - L) < delta) {
            strip.push_back(p);
        }
    }

    // Step 5: 檢查 strip 中可能跨中線的最近點
    int m = strip.size();
    for (int i = 0; i < m; i++) {
        // 對 strip[i] 後面有限個點進行比較 (由於 Y 已排序，只需檢查 Y 差距小於 delta 的點)
        // 若找到更小距離，更新 delta
        for (int j = i + 1; j < m && (strip[j].y - strip[i].y) < delta; j++) {
            delta = min(delta, distancePoints(strip[i], strip[j]));
        }
    }

    return delta;
}

// 封裝主函式：先依 x 與 y 排序，再呼叫遞迴
double closestPair(vector<Point> S) {
    // 1. 建立 Px 與 Py
    vector<Point> Px = S;
    vector<Point> Py = S;
    
    // 2. Px 依 x 排序
    sort(Px.begin(), Px.end(), cmpX);
    
    // 3. Py 依 y 排序
    sort(Py.begin(), Py.end(), cmpY);
    
    // 4. 呼叫 closestPairRecursive(Px, Py)
    return closestPairRecursive(Px, Py); 
}

// =====================================================
// 主程式
// =====================================================
int main() {
    // 測資 1
    vector<Point> case1 = {
        {5,12}, {12,10}, {2,9}, {5,7}, {10,6},
        {9,4}, {6,3}, {11,4}, {1,3}, {6,1}
    };

    // 測資 2
    vector<Point> case2 = {
        {6,0}, {11,3}, {10,9}, {8,5}, {9,7}, {6,10},
        {4,7}, {5,4}, {3,8}, {2,3}, {2,9}, {9,4}
    };

    // ------------------------------
    // Part A: 2D_Maximal
    // ------------------------------
    printPoints(case1, "Input Case 1:");
    vector<Point> ans1 = maximalPoints(case1);
    sort(ans1.begin(), ans1.end(), cmpX);
    printPoints(ans1, "Maximal Points of Case 1:");

    printPoints(case2, "Input Case 2:");
    vector<Point> ans2 = maximalPoints(case2);
    sort(ans2.begin(), ans2.end(), cmpX);
    printPoints(ans2, "Maximal Points of Case 2:");

    // ------------------------------
    // Part B: 2D_Closest_Pair
    // ------------------------------
    cout << "Closest Pair Distance of Case 1: "
         << closestPair(case1) << "\n";

    cout << "Closest Pair Distance of Case 2: "
         << closestPair(case2) << "\n";

    return 0;
}
