#pragma once
#include <vector>
#include <memory>

template <typename T>
class ObjectPool 
{
private:
    std::vector<std::shared_ptr<T>> m_pPool;

public:
    // オブジェクトを取得する
    std::shared_ptr<T> get() {
        if (m_pPool.empty()) {
            return std::make_shared<T>();
        }
        else {
            // 後ろからオブジェクトを取得し、プールから削除する
            auto obj = m_pPool.back();
            m_pPool.pop_back();
            return obj;
        }
    }

    // オブジェクトをプールに戻す
    void release(std::shared_ptr<T> obj) {
        m_pPool.push_back(obj);
    }
};
