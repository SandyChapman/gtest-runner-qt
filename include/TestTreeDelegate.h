#ifndef DELEGATE_H
#define DELEGATE_H

#include <QApplication>
#include <QItemDelegate>
#include <QProgressBar>

class TestTreeDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
    inline void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        // Création d'un QStyleOptionProgressBar qui permettra de dessiner notre barre de progression :
        QStyleOptionProgressBar progressBar;

        progressBar.state = QStyle::State_Enabled;
        progressBar.direction = QApplication::layoutDirection();
        progressBar.fontMetrics = QApplication::fontMetrics();
        progressBar.rect = option.rect;
        progressBar.maximum = 0;
        progressBar.minimum = 0;

        // Gestion du texte :
//        progressBar.text = QString::number(progressBar.progress) + "%";
//        progressBar.textAlignment = Qt::AlignCenter;
//        progressBar.textVisible = true;

        // Affichage de la barre de progression à l'aide du style de l'application :
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBar, painter);
    }
};

#endif // DELEGATE_H
