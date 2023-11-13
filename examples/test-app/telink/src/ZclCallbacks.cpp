/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "AppTask.h"
#include "ColorFormat.h"
#include "PWMDevice.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <lib/support/logging/CHIPLogging.h>
#include <app/util/attribute-storage.h>
LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);

using namespace chip;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OnOff;

void MatterPostAttributeChangeCallback(const chip::app::ConcreteAttributePath & attributePath, uint8_t type, uint16_t size,
                                       uint8_t * value)
{
    static HsvColor_t hsv;
    static XyColor_t xy;
    ClusterId clusterId     = attributePath.mClusterId;
    AttributeId attributeId = attributePath.mAttributeId;

    if (clusterId == OnOff::Id && attributeId == OnOff::Attributes::OnOff::Id)
    {
        ChipLogDetail(Zcl, "Cluster OnOff: attribute OnOff set to %u", *value);
        GetAppTask().SetInitiateAction(*value ? PWMDevice::ON_ACTION : PWMDevice::OFF_ACTION,
                                       static_cast<int32_t>(AppEvent::kEventType_Lighting), value);
    }
    else if (clusterId == LevelControl::Id && attributeId == LevelControl::Attributes::CurrentLevel::Id)
    {
        if (GetAppTask().GetPWMDevice().IsTurnedOn())
        {
            ChipLogDetail(Zcl, "Cluster LevelControl: attribute CurrentLevel set to %u", *value);
            GetAppTask().SetInitiateAction(PWMDevice::LEVEL_ACTION, static_cast<int32_t>(AppEvent::kEventType_Lighting), value);
        }
        else
        {
            ChipLogDetail(Zcl, "LED is off. Try to use move-to-level-with-on-off instead of move-to-level");
        }
    }
    else if (clusterId == ColorControl::Id)
    {
        /* Ignore several attributes that are currently not processed */
        if ((attributeId == ColorControl::Attributes::RemainingTime::Id) ||
            (attributeId == ColorControl::Attributes::EnhancedColorMode::Id) ||
            (attributeId == ColorControl::Attributes::ColorMode::Id))
        {
            return;
        }

        /* XY color space */
        if (attributeId == ColorControl::Attributes::CurrentX::Id || attributeId == ColorControl::Attributes::CurrentY::Id)
        {
            if (attributeId == ColorControl::Attributes::CurrentX::Id)
            {
                xy.x = *reinterpret_cast<uint16_t *>(value);
            }
            else if (attributeId == ColorControl::Attributes::CurrentY::Id)
            {
                xy.y = *reinterpret_cast<uint16_t *>(value);
            }

            ChipLogDetail(Zcl, "New XY color: %u|%u", xy.x, xy.y);
            GetAppTask().SetInitiateAction(PWMDevice::COLOR_ACTION_XY, static_cast<int32_t>(AppEvent::kEventType_Lighting),
                                           (uint8_t *) &xy);
        }
        /* HSV color space */
        else if (attributeId == ColorControl::Attributes::CurrentHue::Id ||
                 attributeId == ColorControl::Attributes::CurrentSaturation::Id ||
                 attributeId == ColorControl::Attributes::EnhancedCurrentHue::Id)
        {
            if (attributeId == ColorControl::Attributes::EnhancedCurrentHue::Id)
            {
                hsv.h = (uint8_t)(((*reinterpret_cast<uint16_t *>(value)) & 0xFF00) >> 8);
                hsv.s = (uint8_t)((*reinterpret_cast<uint16_t *>(value)) & 0xFF);
            }
            else if (attributeId == ColorControl::Attributes::CurrentHue::Id)
            {
                hsv.h = *value;
            }
            else if (attributeId == ColorControl::Attributes::CurrentSaturation::Id)
            {
                hsv.s = *value;
            }
            ChipLogDetail(Zcl, "New HSV color: hue = %u| saturation = %u", hsv.h, hsv.s);
            GetAppTask().SetInitiateAction(PWMDevice::COLOR_ACTION_HSV, static_cast<int32_t>(AppEvent::kEventType_Lighting),
                                           (uint8_t *) &hsv);
        }
        /* Temperature Mireds color space */
        else if (attributeId == ColorControl::Attributes::ColorTemperatureMireds::Id)
        {
            ChipLogDetail(Zcl, "New Temperature Mireds color = %u", *(uint16_t *) value);
            GetAppTask().SetInitiateAction(PWMDevice::COLOR_ACTION_CT, static_cast<int32_t>(AppEvent::kEventType_Lighting), value);
        }
        else
        {
            ChipLogDetail(Zcl, "Ignore ColorControl attribute (%u) that is not currently processed!", attributeId);
        }
    }
}




// using namespace chip;
// using namespace chip::app;
// using namespace chip::app::Clusters;
// using namespace chip::app::Clusters::FanControl::Attributes;

// namespace {

// /*
//  * TODO: This is a stop-gap solution to allow the existing fan control cluster tests to run after changes to
//  * the cluster objects for TE1. This should be removed once #6496 is resolved as it will likely result in a
//  * FanControl delegate added to the SDK.
//  *
//  * FYI... The previous implementation of the FanControl cluster set the speedCurrent/percentCurrent when it received
//  * speedSetting/percentSetting. The new implementation of the FanControl cluster does not do this as this should
//  * really be done by the application.
//  */

// class FanAttrAccess : public AttributeAccessInterface
// {
// public:
//     // Register for the FanControl cluster on all endpoints.
//     FanAttrAccess() : AttributeAccessInterface(Optional<EndpointId>::Missing(), FanControl::Id) {}

//     CHIP_ERROR Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder) override;

// private:
//     CHIP_ERROR ReadPercentCurrent(EndpointId endpoint, AttributeValueEncoder & aEncoder);
//     CHIP_ERROR ReadSpeedCurrent(EndpointId endpoint, AttributeValueEncoder & aEncoder);
// };

// CHIP_ERROR FanAttrAccess::ReadPercentCurrent(EndpointId endpoint, AttributeValueEncoder & aEncoder)
// {
//     // Return PercentSetting attribute value for now
//     DataModel::Nullable<uint8_t> percentSetting;
//     PercentSetting::Get(endpoint, percentSetting);
//     uint8_t ret = 0;
//     if (!percentSetting.IsNull())
//     {
//         ret = percentSetting.Value();
//     }

//     return aEncoder.Encode(ret);
// }

// CHIP_ERROR FanAttrAccess::ReadSpeedCurrent(EndpointId endpoint, AttributeValueEncoder & aEncoder)
// {
//     // Return SpeedCurrent attribute value for now
//     DataModel::Nullable<uint8_t> speedSetting;
//     SpeedSetting::Get(endpoint, speedSetting);
//     uint8_t ret = 0;
//     if (!speedSetting.IsNull())
//     {
//         ret = speedSetting.Value();
//     }

//     return aEncoder.Encode(ret);
// }

// FanAttrAccess gAttrAccess;

// CHIP_ERROR FanAttrAccess::Read(const ConcreteReadAttributePath & aPath, AttributeValueEncoder & aEncoder)
// {
//     VerifyOrDie(aPath.mClusterId == FanControl::Id);

//     switch (aPath.mAttributeId)
//     {
//     case SpeedCurrent::Id:
//         return ReadSpeedCurrent(aPath.mEndpointId, aEncoder);
//     case PercentCurrent::Id:
//         return ReadPercentCurrent(aPath.mEndpointId, aEncoder);
//     default:
//         break;
//     }
//     return CHIP_NO_ERROR;
// }
// } // anonymous namespace

// void emberAfFanControlClusterInitCallback(EndpointId endpoint)
// {
//     uint32_t featureMap = 0;

//     LOG_DBG("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
//     LOG_DBG("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
//     LOG_DBG("emberAfFanControlClusterInitCallback(),%u",endpoint);


//     featureMap |= to_underlying(FanControl::Feature::kMultiSpeed);
//     featureMap |= to_underlying(FanControl::Feature::kMultiSpeed);
//     featureMap |= to_underlying(FanControl::Feature::kAuto);

//     FeatureMap::Set(endpoint, featureMap);

//     registerAttributeAccessOverride(&gAttrAccess);
// }








// /** @brief OnOff Cluster Init
//  *
//  * This function is called when a specific cluster is initialized. It gives the
//  * application an opportunity to take care of cluster initialization procedures.
//  * It is called exactly once for each endpoint where cluster is present.
//  *
//  * @param endpoint   Ver.: always
//  *
//  */
// void emberAfOnOffClusterInitCallback(EndpointId endpoint)
// {
//     EmberAfStatus status;
//     bool storedValue;


//     LOG_DBG("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
//     LOG_DBG("emberAfOnOffClusterInitCallback(),%u",endpoint);


//     // Read storedValue on/off value
//     status = Attributes::OnOff::Get(1, &storedValue);
//     if (status == EMBER_ZCL_STATUS_SUCCESS)
//     {
//         // Set actual state to stored before reboot
//         GetAppTask().GetPWMDevice().Set(storedValue);
//     }

//     GetAppTask().UpdateClusterState();
// }
