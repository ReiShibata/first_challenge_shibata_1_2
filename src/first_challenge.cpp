#include "first_challenge/first_challenge.hpp"
#include<cmath>

FirstChallenge::FirstChallenge() : Node("first_challenge_node")
{
    // --- パブリッシャーの作成 ---
    // トピック名: "cmd_vel"
    cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

    // --- サブスクライバーの作成 ---
    // トピック名: "odom"
    // 実行する関数: odometry_callback (この後、クラス内に定義する必要があります)
    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>("odom", 10, std::bind(&FirstChallenge::odometry_callback, this, std::placeholders::_1));

    //--- パラメータの設定 ---
    velocity_ = 0.3;
    goal_dist_ = 1.0;
}

void FirstChallenge::odometry_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
{
    // ここに受け取った時の処理を書く
    odom_ = *msg;  // 受け取ったメッセージをクラス内の変数に保存
}

bool FirstChallenge::can_move()
{
    return odom_.has_value();
}

bool FirstChallenge::is_goal()
{
    // ゴールに到達したかの判定
    return calc_distance() >= goal_dist_;
}

double FirstChallenge::calc_distance()
{
    if(!odom_.has_value()) 
    {
        return 0.0;
    }

    // 現在のx, y座標を取得
    double x = odom_->pose.pose.position.x;
    double y = odom_->pose.pose.position.y;

    // 原点(0,0)からの直線距離を計算: sqrt(x^2 + y^2)
    return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
}

void FirstChallenge::run(float velocity, float omega)
{
    // 速度データを格納
    cmd_vel_.linear.x = velocity;
    cmd_vel_.angular.z = omega;

    // パブリッシュを実行
    cmd_vel_pub_->publish(cmd_vel_);
}

void FirstChallenge::set_cmd_vel()
{
    // ゴール判定
    if(is_goal())
    {
        // 停止
        run(0.0, 0.0);
    }
    else
    {
        // 直進 (0.1m/s, 0.0rad/s)
        run(velocity_, 0.0);
    }
}
