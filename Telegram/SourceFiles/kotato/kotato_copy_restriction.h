/*
This file is part of Kotatogram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/kotatogram/kotatogram-desktop/blob/dev/LEGAL
*/
#pragma once

class PeerData;
class HistoryItem;

namespace Kotato {

[[nodiscard]] bool CopyRestrictionBypassed();

[[nodiscard]] bool HasCopyRestriction(
	not_null<const PeerData*> peer,
	const HistoryItem *item = nullptr);

[[nodiscard]] bool HasCopyMediaRestriction(
	not_null<const PeerData*> peer,
	not_null<const HistoryItem*> item);

} // namespace Kotato
