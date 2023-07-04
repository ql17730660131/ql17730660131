#include "ql_rpccontroller.h"

QlrpcController::QlrpcController()
{
    m_failed = false;
    m_errText = "";
}

void QlrpcController::Reset()
{
    m_failed = false;
    m_errText = "";
}

bool QlrpcController::Failed() const
{
    return m_failed;
}

std::string QlrpcController::ErrorText() const
{
    return m_errText;
}

void QlrpcController::SetFailed(const std::string &reason)
{
    m_failed = true;
    m_errText = reason;
}

// 目前未实现具体的功能
void QlrpcController::StartCancel() {}
bool QlrpcController::IsCanceled() const { return false; }
void QlrpcController::NotifyOnCancel(google::protobuf::Closure *callback) {}