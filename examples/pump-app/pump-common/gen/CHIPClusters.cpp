/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
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

// THIS FILE IS GENERATED BY ZAP
#include "CHIPClusters.h"

#include <cstdint>

#include <app/chip-zcl-zpro-codec-api.h>
#include <lib/support/Span.h>

namespace chip {
namespace Controller {

// TODO(#4502): onCompletion is not used by IM for now.
// TODO(#4503): length should be passed to commands when byte string is in argument list.
// TODO(#4503): Commands should take group id as an argument.

// Identify Cluster Commands
CHIP_ERROR IdentifyCluster::Identify(Callback::Cancelable * onSuccessCallback, Callback::Cancelable * onFailureCallback,
                                     uint16_t identifyTime)
{
#if CHIP_ENABLE_INTERACTION_MODEL
    VerifyOrReturnError(mDevice != nullptr, CHIP_ERROR_INCORRECT_STATE);
    (void) onSuccessCallback;
    (void) onFailureCallback;

    app::CommandPathParams cmdParams = { mEndpoint, /* group id */ 0, mClusterId, kIdentifyCommandId,
                                         (chip::app::CommandPathFlags::kEndpointIdValid) };
    app::Command * ZCLcommand        = mDevice->GetCommandSender();

    ReturnErrorOnFailure(ZCLcommand->PrepareCommand(&cmdParams));

    TLV::TLVWriter * writer = ZCLcommand->GetCommandDataElementTLVWriter();
    uint8_t argSeqNumber    = 0;
    // identifyTime: int16u
    ReturnErrorOnFailure(writer->Put(TLV::ContextTag(argSeqNumber++), identifyTime));

    ReturnErrorOnFailure(ZCLcommand->FinishCommand());

    return mDevice->SendCommands();
#else
    uint8_t seqNum                            = mDevice->GetNextSequenceNumber();
    System::PacketBufferHandle encodedCommand = encodeIdentifyClusterIdentifyCommand(seqNum, mEndpoint, identifyTime);
    return SendCommand(seqNum, std::move(encodedCommand), onSuccessCallback, onFailureCallback);
#endif
}

CHIP_ERROR IdentifyCluster::IdentifyQuery(Callback::Cancelable * onSuccessCallback, Callback::Cancelable * onFailureCallback)
{
#if CHIP_ENABLE_INTERACTION_MODEL
    VerifyOrReturnError(mDevice != nullptr, CHIP_ERROR_INCORRECT_STATE);
    (void) onSuccessCallback;
    (void) onFailureCallback;

    app::CommandPathParams cmdParams = { mEndpoint, /* group id */ 0, mClusterId, kIdentifyQueryCommandId,
                                         (chip::app::CommandPathFlags::kEndpointIdValid) };
    app::Command * ZCLcommand        = mDevice->GetCommandSender();

    ReturnErrorOnFailure(ZCLcommand->PrepareCommand(&cmdParams));

    // Command takes no arguments.

    ReturnErrorOnFailure(ZCLcommand->FinishCommand());

    return mDevice->SendCommands();
#else
    uint8_t seqNum                            = mDevice->GetNextSequenceNumber();
    System::PacketBufferHandle encodedCommand = encodeIdentifyClusterIdentifyQueryCommand(seqNum, mEndpoint);
    return SendCommand(seqNum, std::move(encodedCommand), onSuccessCallback, onFailureCallback);
#endif
}

// Identify Cluster Attributes
CHIP_ERROR IdentifyCluster::DiscoverAttributes(Callback::Cancelable * onSuccessCallback, Callback::Cancelable * onFailureCallback)
{
    uint8_t seqNum                            = mDevice->GetNextSequenceNumber();
    System::PacketBufferHandle encodedCommand = encodeIdentifyClusterDiscoverAttributes(seqNum, mEndpoint);
    return SendCommand(seqNum, std::move(encodedCommand), onSuccessCallback, onFailureCallback);
}
CHIP_ERROR IdentifyCluster::ReadAttributeIdentifyTime(Callback::Cancelable * onSuccessCallback,
                                                      Callback::Cancelable * onFailureCallback)
{
    uint8_t seqNum                            = mDevice->GetNextSequenceNumber();
    System::PacketBufferHandle encodedCommand = encodeIdentifyClusterReadIdentifyTimeAttribute(seqNum, mEndpoint);
    return SendCommand(seqNum, std::move(encodedCommand), onSuccessCallback, onFailureCallback);
}

CHIP_ERROR IdentifyCluster::WriteAttributeIdentifyTime(Callback::Cancelable * onSuccessCallback,
                                                       Callback::Cancelable * onFailureCallback, uint16_t value)
{
    uint8_t seqNum                            = mDevice->GetNextSequenceNumber();
    System::PacketBufferHandle encodedCommand = encodeIdentifyClusterWriteIdentifyTimeAttribute(seqNum, mEndpoint, value);
    return SendCommand(seqNum, std::move(encodedCommand), onSuccessCallback, onFailureCallback);
}

CHIP_ERROR IdentifyCluster::ReadAttributeClusterRevision(Callback::Cancelable * onSuccessCallback,
                                                         Callback::Cancelable * onFailureCallback)
{
    uint8_t seqNum                            = mDevice->GetNextSequenceNumber();
    System::PacketBufferHandle encodedCommand = encodeIdentifyClusterReadClusterRevisionAttribute(seqNum, mEndpoint);
    return SendCommand(seqNum, std::move(encodedCommand), onSuccessCallback, onFailureCallback);
}

} // namespace Controller
} // namespace chip
