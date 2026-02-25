#pragma once

enum class BULLET_TYPE
{
    NORMAL,          // 通常弾（アサルトとか）
    EXPLOSION_01,    // 爆発系（ロケットランチャーとか）
    LASER_01,        // レーザー系（直線）



    NUM,
};


class BulletManager
{
private:
    std::vector<std::shared_ptr<class Bullet>> m_pBulletList;


public:

};

