#include "twc_protocol.h"
#include "twc_charger.h"
#include "twc_connector.h"

#include <algorithm>

namespace esphome {
namespace twc_controller {

// Note: chargers_ is now a dynamic vector (std::vector<std::unique_ptr<TwcCharger>>)
// declared in the header (twc_protocol.h). This file implements the functions
// that operate on that vector and handles creation/iteration safely.

bool TwcProtocol::ChargersConnected() const {
  // Return true if at least one discovered charger reports a connection.
  for (const auto &charger_ptr : this->chargers_) {
    if (!charger_ptr) continue;
    if (charger_ptr->IsConnected())
      return true;
  }
  return false;
}

TwcConnector *TwcProtocol::GetConnector(std::size_t charger_index, std::size_t connector_index) {
  // Bounds check on charger index
  if (charger_index >= this->chargers_.size())
    return nullptr;

  const auto &charger_ptr = this->chargers_[charger_index];
  if (!charger_ptr)
    return nullptr;

  // Delegate to the charger to obtain its connector (it should handle connector bounds)
  return charger_ptr->GetConnector(connector_index);
}

// Example helper used when discovering/creating chargers. This keeps creation
// and ownership in the dynamic vector. Other parts of the protocol can call
// AddDiscoveredCharger to register new chargers as they are discovered.
void TwcProtocol::AddDiscoveredCharger(std::unique_ptr<TwcCharger> charger) {
  if (!charger) return;
  this->chargers_.push_back(std::move(charger));
}

// Optional: Clear discovered chargers (release ownership)
void TwcProtocol::ClearDiscoveredChargers() {
  this->chargers_.clear();
}

}  // namespace twc_controller
}  // namespace esphome
