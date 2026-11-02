//
// Created by LEGION on 2021/10/4.
//
#include "ChassisTask.h"

PID_Regulator_t pidRegulator = {//此为储存pid参数的结构体，四个底盘电机共用
        .kp = 60,
        .ki = 0,
        .kd = 0,
        .componentKpMax = 16384,
        .componentKiMax = 0,
        .componentKdMax = 0,
        .outputMax = 8000 //3508电机输出电流上限，调试先限流，熟练后再调大，勿超过16384
};

MOTOR_INIT_t chassisMotorInit = {//四个底盘电机共用的初始化结构体
        .speedPIDp = &pidRegulator,
        .anglePIDp = nullptr,
        ._motorID = MOTOR_ID_1,
        .reductionRatio = 19.0f,
        .ctrlType = SPEED_Single,
};
Motor CMFL(MOTOR_ID_1,&chassisMotorInit);//定义左前轮电机
Motor CMFR(MOTOR_ID_2,&chassisMotorInit);//定义右前轮电机
Motor CMBL(MOTOR_ID_3,&chassisMotorInit);//定义左后轮电机
Motor CMBR(MOTOR_ID_4,&chassisMotorInit);//定义右后轮电机

uint8_t ChassisStopFlag = 1;
float FBVelocity,LRVelocity,RTVelocity;

/*
 * 四个轮子的方向修正系数。
 * 如果某个轮子实际转向和期望相反，只把对应的一行改成 -1.0f，
 * 不要去改下面的解算公式。
 */
#define CMFL_DIR 1.0f
#define CMFR_DIR 1.0f
#define CMBL_DIR 1.0f
#define CMBR_DIR 1.0f

void ChassisStart(){

}
/**
 * @brief 底盘任务的处理函数，定时执行
 * @callergraph void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) in Device.cpp
 */
void ChassisHandle() {
    if(ChassisStopFlag == 0) {
        WheelsSpeedCalc(FBVelocity, LRVelocity, RTVelocity);
    }
    CMFL.Handle();
    CMFR.Handle();
    CMBL.Handle();
    CMBR.Handle();
}
/**
 * @brief 用于控制任务控制底盘速度
 * @param _fbV 底盘前后方向速度
 * @param _lrV 底盘左右方向速度
 * @param _rtV 底盘旋转速度
 */
void ChassisSetVelocity(float _fbV,float _lrV,float _rtV){
    ChassisStopFlag = 0;
    FBVelocity = _fbV;
    LRVelocity = _lrV;
    RTVelocity = _rtV;
}
/**
 * @brief 执行急停模式的底盘任务处理
 */
void ChassisStop(){
    ChassisStopFlag = 1;
    CMFL.Stop();
    CMFR.Stop();
    CMBL.Stop();
    CMBR.Stop();
}

void WheelsSpeedCalc(float fbVelocity, float lrVelocity, float rtVelocity) {
    float CMFLSpeed, CMFRSpeed, CMBLSpeed, CMBRSpeed;

    rtVelocity = RPM2RADpS(rtVelocity);

    //计算四个轮子线速度，单位：m/s
    /**
     * @brief 麦轮逆运动学解算：把底盘的前后/左右/旋转速度分解到四个轮子。
     *        约定：fbVelocity 向前为正，lrVelocity 向右为正，
     *              rtVelocity 顺时针为正（对应左摇杆向右）。
     *        (TRACK_WIDTH + WHEEL_BASE) / 2 是麦轮底盘的等效旋转半径。
     */
    const float rotateRadius = (TRACK_WIDTH + WHEEL_BASE) / 2.0f;

    CMFLSpeed = fbVelocity + lrVelocity + rtVelocity * rotateRadius;
    CMFRSpeed = fbVelocity - lrVelocity - rtVelocity * rotateRadius;
    CMBLSpeed = fbVelocity - lrVelocity + rtVelocity * rotateRadius;
    CMBRSpeed = fbVelocity + lrVelocity - rtVelocity * rotateRadius;

    //计算四个轮子角速度，单位：rad/s
    CMFLSpeed = CMFLSpeed /(WHEEL_DIAMETER/2.0f);
    CMFRSpeed = CMFRSpeed /(WHEEL_DIAMETER/2.0f);
    CMBLSpeed = CMBLSpeed /(WHEEL_DIAMETER/2.0f);
    CMBRSpeed = CMBRSpeed /(WHEEL_DIAMETER/2.0f);
    //控制底盘电机转速（DIR 只用于修正单个轮子的安装/接线方向）
    CMFL.SetTargetSpeed(CMFL_DIR * RADpS2RPM(CMFLSpeed));
    CMFR.SetTargetSpeed(CMFR_DIR * RADpS2RPM(CMFRSpeed));
    CMBL.SetTargetSpeed(CMBL_DIR * RADpS2RPM(CMBLSpeed));
    CMBR.SetTargetSpeed(CMBR_DIR * RADpS2RPM(CMBRSpeed));
}
