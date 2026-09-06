/*
This file is part of Kotatogram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/kotatogram/kotatogram-desktop/blob/dev/LEGAL
*/
#pragma once

#include "history/view/history_view_element.h"

namespace HistoryView {

class CollapsedMessage final : public Element {
public:
	CollapsedMessage(
		not_null<ElementDelegate*> delegate,
		not_null<HistoryItem*> data,
		Element *replacing);

	int marginTop() const override;
	int marginBottom() const override;
	void draw(Painter &p, const PaintContext &context) const override;
	PointState pointState(QPoint point) const override;
	TextState textState(
		QPoint point,
		StateRequest request) const override;
	void updatePressed(QPoint point) override;
	TextForMimeData selectedText(TextSelection selection) const override;
	SelectedQuote selectedQuote(TextSelection selection) const override;
	TextSelection selectionFromQuote(
		const SelectedQuote &quote) const override;
	TextSelection adjustSelection(
		TextSelection selection,
		TextSelectType type) const override;

	QRect innerGeometry() const override;

	void animateReaction(Ui::ReactionFlyAnimationArgs &&args) override;

private:
	[[nodiscard]] QRect rowGeometry() const;

	QSize performCountOptimalSize() override;
	QSize performCountCurrentSize(int newWidth) override;

	Ui::Text::String _name;
	Ui::Text::String _suffix;
	ClickHandlerPtr _link;

};

} // namespace HistoryView
