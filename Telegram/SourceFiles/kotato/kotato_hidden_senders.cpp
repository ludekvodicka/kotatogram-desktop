/*
This file is part of Kotatogram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/kotatogram/kotatogram-desktop/blob/dev/LEGAL
*/
#include "kotato/kotato_hidden_senders.h"

#include "kotato/kotato_settings.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

namespace Kotato {
namespace {

constexpr auto kKey = "hidden_senders";

} // namespace

HiddenSenders::HiddenSenders(not_null<Main::Session*> session)
: _session(session) {
}

PeerId HiddenSenders::ChatKey(not_null<PeerData*> chat) {
	const auto migrated = chat->migrateTo();
	return migrated ? migrated->id : chat->id;
}

bool HiddenSenders::isHidden(
		not_null<PeerData*> chat,
		not_null<PeerData*> sender) const {
	loadOnce();
	return _hidden.contains(Key{ ChatKey(chat), sender->id });
}

bool HiddenSenders::isCollapsed(not_null<const HistoryItem*> item) const {
	if (item->isService() || item->out()) {
		return false;
	}
	const auto from = item->from();
	if (from->isSelf()) {
		return false;
	}
	return isHidden(item->history()->peer, from)
		&& !_expanded.contains(item->fullId());
}

bool HiddenSenders::isExpanded(FullMsgId id) const {
	return _expanded.contains(id);
}

void HiddenSenders::toggleHidden(
		not_null<PeerData*> chat,
		not_null<PeerData*> sender) {
	loadOnce();
	const auto key = Key{ ChatKey(chat), sender->id };
	if (_hidden.contains(key)) {
		_hidden.remove(key);
	} else {
		_hidden.emplace(key);
	}
	save();
	refreshViews(chat, sender);
}

void HiddenSenders::toggleExpanded(not_null<HistoryItem*> item) {
	const auto id = item->fullId();
	if (_expanded.contains(id)) {
		_expanded.remove(id);
	} else {
		_expanded.emplace(id);
	}
	// The view class changes, so a resize request is not enough.
	_session->data().requestItemViewRefresh(item);
}

void HiddenSenders::refreshViews(
		not_null<PeerData*> chat,
		not_null<PeerData*> sender) {
	const auto refresh = [&](PeerId peerId) {
		_session->data().enumerateMessages(peerId, [&](
				not_null<HistoryItem*> item) {
			if (item->from() != sender) {
				return;
			}
			_expanded.remove(item->fullId());
			_session->data().requestItemViewRefresh(item);
			item->invalidateChatListEntry();
		});
	};
	refresh(chat->id);
	if (const auto migrated = chat->migrateFrom()) {
		refresh(migrated->id);
	}
	if (const auto migrated = chat->migrateTo()) {
		refresh(migrated->id);
	}
}

void HiddenSenders::loadOnce() const {
	if (_loaded) {
		return;
	}
	_loaded = true;
	const auto list = JsonSettings::GetJsonArray(
		kKey,
		_session->userId().bare,
		_session->isTestMode());
	for (const auto &value : list) {
		const auto pair = value.toArray();
		if (pair.size() != 2) {
			continue;
		}
		const auto chat = PeerId(pair[0].toString().toULongLong());
		const auto sender = PeerId(pair[1].toString().toULongLong());
		if (chat && sender) {
			_hidden.emplace(chat, sender);
		}
	}
}

void HiddenSenders::save() const {
	auto list = QJsonArray();
	for (const auto &[chat, sender] : _hidden) {
		list << QJsonArray{
			QString::number(chat.value),
			QString::number(sender.value) };
	}
	JsonSettings::Set(
		kKey,
		list,
		_session->userId().bare,
		_session->isTestMode());
	JsonSettings::Write();
}

} // namespace Kotato
