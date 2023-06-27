#pragma once

#include <Eigen/Eigen>

namespace ArchViz
{
    using FVector2 = Eigen::Vector2f;
    using FVector3 = Eigen::Vector3f;
    using FVector4 = Eigen::Vector4f;

    using DVector2 = Eigen::Vector2d;
    using DVector3 = Eigen::Vector3d;
    using DVector4 = Eigen::Vector4d;

    using FMatrix2 = Eigen::Matrix2f;
    using FMatrix3 = Eigen::Matrix3f;
    using FMatrix4 = Eigen::Matrix4f;
    using FMatrixX = Eigen::MatrixXf;

    using DMatrix2 = Eigen::Matrix2d;
    using DMatrix3 = Eigen::Matrix3d;
    using DMatrix4 = Eigen::Matrix4d;
    using DMatrixX = Eigen::MatrixXd;

    using FQuaternion = Eigen::Quaternionf;
    using DQuaternion = Eigen::Quaterniond;

    using FTranslation2 = Eigen::Translation2f;
    using FTranslation3 = Eigen::Translation3f;

    using DTranslation2 = Eigen::Translation2d;
    using DTranslation3 = Eigen::Translation3d;

    using FAngleAxis = Eigen::AngleAxisf;
    using DAngleAxis = Eigen::AngleAxisd;

    using FAffine = Eigen::Affine3f;
    using DAffine = Eigen::Affine3d;

    using FIsometry2 = Eigen::Isometry2f;
    using FIsometry3 = Eigen::Isometry3f;

    using DIsometry2 = Eigen::Isometry2d;
    using DIsometry3 = Eigen::Isometry3d;

    using FProjective2 = Eigen::Projective2f;
    using FProjective3 = Eigen::Projective3f;

    using DProjective2 = Eigen::Projective2d;
    using DProjective3 = Eigen::Projective3d;

    using FTransform2 = FMatrix3;
    using FTransform3 = FMatrix4;
} // namespace Piccolo