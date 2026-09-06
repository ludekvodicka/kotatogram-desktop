/*
This file is part of Kotatogram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/kotatogram/kotatogram-desktop/blob/dev/LEGAL
*/
#include "history/view/history_view_collapsed_message.h"

#include "kotato/kotato_hidden_senders.h"
#include "kotato/kotato_lang.h"
#include "core/click_handler_types.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "history/view/history_view_cursor_state.h"
#include "main/main_session.h"
#include "ui/chat/chat_style.h"
#include "ui/painter.h"
#include "ui/text/text_options.h"
#include "styles/style_chat.h"

namespace HistoryView {

CollapsedMessage::CollapsedMessage(
	not_null<ElementDelegate*> delegate,
	not_null<HistoryItem*> data,
	Element *replacing)
: Element(delegate, data, replacing, Flag::CollapsedSender) {
	const auto session = &data->history()->session();
	const auto itemId = data->fullId();
	_link = std::make_shared<LambdaClickHandler>([=] {
		if (const auto item = session->data().message(itemId)) {
			session->hiddenSenders().toggleExpanded(item);
		}
	});
}

int CollapsedMessage::marginTop() const {
	auto result = isHidden() ? 0 : st::msgServiceMargin.top();
	result += displayedDateHeight();
	if (const auto bar = Get<UnreadBar>()) {
		result += bar->height();
	}
	if (const auto bar = Get<ForumThreadBar>()) {
		result += bar->height();
	}
	if (const auto margins = Get<ViewAddedMargins>()) {
		result += margins->top;
	}
	return result;
}

int CollapsedMessage::marginBottom() const {
	auto result = isHidden() ? 0 : st::msgServiceMargin.bottom();
	if (const auto margins = Get<ViewAddedMargins>()) {
		result += margins->bottom;
	}
	return result;
}

QSize CollapsedMessage::performCountOptimalSize() {
	const auto item = data();
	_name.setText(
		st::msgNameStyle,
		item->from()->name(),
		Ui::NameTextOptions());
	_suffix.setText(st::defaultTextStyle, ktr("ktg_hidden_message"));
	const auto width = st::msgMargin.left()
		+ _name.maxWidth()
		+ st::normalFont->spacew
		+ _suffix.maxWidth()
		+ st::msgMargin.right();
	const auto height = st::msgServicePadding.top()
		+ st::msgServiceFont->height
		+ st::msgServicePadding.bottom();
	return { std::max(width, int(st::msgMinWidth)), height };
}

QSize CollapsedMessage::performCountCurrentSize(int newWidth) {
	if (isHidden()) {
		return { newWidth, marginTop() };
	}
	return { newWidth, marginTop() + minHeight() + marginBottom() };
}

QRect CollapsedMessage::rowGeometry() const {
	return QRect(
		st::msgMargin.left(),
		marginTop(),
		std::max(width() - st::msgMargin.left() - st::msgMargin.right(), 1),
		minHeight());
}

void CollapsedMessage::draw(Painter &p, const PaintContext &context) const {
	if (const auto bar = Get<UnreadBar>()) {
		auto unreadbarh = bar->height();
		auto aboveh = 0;
		if (const auto date = Get<DateBadge>()) {
			aboveh += date->height();
		}
		if (const auto thread = Get<ForumThreadBar>()) {
			aboveh += thread->height();
		}
		if (context.clip.intersects(QRect(0, aboveh, width(), unreadbarh))) {
			p.translate(0, aboveh);
			bar->paint(
				p,
				context,
				0,
				width(),
				delegate()->elementChatMode());
			p.translate(0, -aboveh);
		}
	}
	if (isHidden()) {
		return;
	}
	const auto g = rowGeometry();
	if (g.width() < 1) {
		return;
	}
	paintHighlight(p, context, g.height());

	const auto textTop = g.top() + st::msgServicePadding.top();
	p.setPen(FromNameFg(context, colorIndex(), colorCollectible()));
	_name.drawLeftElided(p, g.left(), textTop, g.width(), width());

	const auto suffixLeft = g.left()
		+ _name.maxWidth()
		+ st::normalFont->spacew;
	const auto suffixWidth = g.left() + g.width() - suffixLeft;
	if (suffixWidth > 0) {
		p.setPen(context.st->msgServiceFg());
		_suffix.drawLeftElided(p, suffixLeft, textTop, suffixWidth, width());
	}
}

PointState CollapsedMessage::pointState(QPoint point) const {
	// Outside keeps drag-selection from picking the row up; a right-click
	// still resolves the item by its y-range, so the context menu opens.
	return PointState::Outside;
}

TextState CollapsedMessage::textState(
		QPoint point,
		StateRequest request) const {
	auto result = TextState(data());
	if (!isHidden() && rowGeometry().contains(point)) {
		result.link = _link;
	}
	return result;
}

void CollapsedMessage::updatePressed(QPoint point) {
}

TextForMimeData CollapsedMessage::selectedText(TextSelection selection) const {
	return TextForMimeData();
}

SelectedQuote CollapsedMessage::selectedQuote(TextSelection selection) const {
	return {};
}

TextSelection CollapsedMessage::selectionFromQuote(
		const SelectedQuote &quote) const {
	return {};
}

TextSelection CollapsedMessage::adjustSelection(
		TextSelection selection,
		TextSelectType type) const {
	return selection;
}

QRect CollapsedMessage::innerGeometry() const {
	return rowGeometry();
}

void CollapsedMessage::animateReaction(Ui::ReactionFlyAnimationArgs &&args) {
}

} // namespace HistoryView
