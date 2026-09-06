/*
This file is part of Kotatogram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/kotatogram/kotatogram-desktop/blob/dev/LEGAL
*/
#include "kotato/kotato_copy_restriction.h"

#include "kotato/kotato_settings.h"
#include "data/data_peer.h"
#include "history/history_item.h"

namespace Kotato {

bool CopyRestrictionBypassed() {
	return JsonSettings::GetBool("bypass_copy_restriction");
}

bool HasCopyRestriction(
		not_null<const PeerData*> peer,
		const HistoryItem *item) {
	if (CopyRestrictionBypassed()) {
		return false;
	}
	return !peer->allowsForwarding() || (item && item->forbidsForward());
}

bool HasCopyMediaRestriction(
		not_null<const PeerData*> peer,
		not_null<const HistoryItem*> item) {
	if (HasCopyRestriction(peer, item)) {
		return true;
	}
	// forbidsSaving() is forbidsForward() plus unpaid extended media.
	// The bypass removes only the NoForwards half of it.
	return item->forbidsSaving()
		&& !(CopyRestrictionBypassed() && item->forbidsForward());
}

} // namespace Kotato
