/*
This file is part of Kotatogram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/kotatogram/kotatogram-desktop/blob/dev/LEGAL
*/
#pragma once

class PeerData;
class HistoryItem;

namespace Main {
class Session;
} // namespace Main

namespace Kotato {

class HiddenSenders final {
public:
	explicit HiddenSenders(not_null<Main::Session*> session);

	[[nodiscard]] bool isHidden(
		not_null<PeerData*> chat,
		not_null<PeerData*> sender) const;
	void toggleHidden(
		not_null<PeerData*> chat,
		not_null<PeerData*> sender);

	[[nodiscard]] bool isCollapsed(not_null<const HistoryItem*> item) const;
	[[nodiscard]] bool isExpanded(FullMsgId id) const;
	void toggleExpanded(not_null<HistoryItem*> item);

private:
	using Key = std::pair<PeerId, PeerId>; // (chat after migration, sender)

	[[nodiscard]] static PeerId ChatKey(not_null<PeerData*> chat);
	void loadOnce() const;
	void save() const;
	void refreshViews(
		not_null<PeerData*> chat,
		not_null<PeerData*> sender);

	const not_null<Main::Session*> _session;
	mutable base::flat_set<Key> _hidden;
	mutable bool _loaded = false;
	base::flat_set<FullMsgId> _expanded;

};

} // namespace Kotato
